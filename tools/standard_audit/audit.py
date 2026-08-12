#!/usr/bin/env python3
"""Structural C++23 synopsis audit for FTL.

The standard input is a private, extracted HTML tree.  Reports retain only
section references and declaration-sized snippets.
"""

from __future__ import annotations

import argparse
import dataclasses
import difflib
import html
import json
import os
import re
import subprocess
import sys
import tempfile
from collections import Counter, defaultdict
from concurrent.futures import ProcessPoolExecutor, as_completed
from functools import lru_cache
from html.parser import HTMLParser
from pathlib import Path
from typing import Any, Iterable


DECL_KINDS = {
    "FunctionDecl": "function",
    "CXXMethodDecl": "function",
    "CXXConstructorDecl": "constructor",
    "CXXDestructorDecl": "destructor",
    "CXXConversionDecl": "conversion",
    "CXXDeductionGuideDecl": "deduction_guide",
    "ClassTemplateDecl": "class_template",
    "ClassTemplateSpecializationDecl": "specialization",
    "TypeAliasDecl": "alias",
    "TypeAliasTemplateDecl": "alias_template",
    "ConceptDecl": "concept",
    "VarDecl": "variable",
}
AST_WRAPPERS = {"FunctionTemplateDecl"}
NOISE_LINES = (
    "ISO/IEC 14882:2024", "BS ISO/IEC 14882:2024", "All rights reserved",
)


@dataclasses.dataclass
class Declaration:
    source: str
    header: str
    mode: str
    kind: str
    name: str
    scope: str
    raw: str
    normalized: str
    signature: str
    template_arity: int = 0
    template_defaults: int = 0
    constraints: str = ""
    qualifiers: list[str] = dataclasses.field(default_factory=list)
    location: str = ""
    section: str = ""
    confidence: str = "HIGH"

    def identity(self) -> tuple[str, str, str]:
        return self.kind, self.scope, self.name


@dataclasses.dataclass
class Warning:
    source: str
    header: str
    location: str
    message: str


@dataclasses.dataclass
class Issue:
    category: str
    confidence: str
    header: str
    message: str
    standard: Declaration | None = None
    ftl: Declaration | None = None
    mode: str = ""
    override: dict[str, Any] | None = None


class ClauseParser(HTMLParser):
    def __init__(self) -> None:
        super().__init__(convert_charrefs=True)
        self.in_clause = False
        self.parts: list[str] = []

    def handle_starttag(self, tag: str, attrs: list[tuple[str, str | None]]) -> None:
        if tag == "div" and dict(attrs).get("class") == "clause":
            self.in_clause = True
        elif self.in_clause and tag in {"br", "p", "li", "tr"}:
            self.parts.append("\n")

    def handle_endtag(self, tag: str) -> None:
        if self.in_clause and tag == "div":
            self.in_clause = False

    def handle_data(self, data: str) -> None:
        if self.in_clause:
            self.parts.append(data)

    @property
    def text(self) -> str:
        return html.unescape("".join(self.parts))


def clean_clause(text: str) -> str:
    lines: list[str] = []
    for line in text.replace("\xa0", " ").splitlines():
        s = line.strip()
        if not s or any(x in s for x in NOISE_LINES) or s.isdigit():
            continue
        lines.append(s)
    return "\n".join(lines)


def normalize_space(text: str) -> str:
    text = text.replace("ftl::", "std::")
    text = re.sub(r"\b(class|typename)\s+(?=T\d*\b)", "typename ", text)
    text = re.sub(r"\s+", " ", text).strip()
    text = re.sub(r"\s*([<>{}(),;*&=])\s*", r"\1", text)
    text = text.replace("&&", " && ").replace("||", " || ")
    return re.sub(r"\s+", " ", text).strip()


def normalize_constraint(text: str) -> str:
    text = normalize_space(text)
    while text.startswith("(") and text.endswith(")"):
        depth = 0
        encloses = True
        for i, ch in enumerate(text):
            depth += ch == "("
            depth -= ch == ")"
            if depth == 0 and i != len(text) - 1:
                encloses = False
                break
        if not encloses:
            break
        text = text[1:-1].strip()
    # Sorting a top-level conjunction is safe; disjunctions retain order/form.
    parts, depth, start = [], 0, 0
    for match in re.finditer(r"&&|[<>()]", text):
        token = match.group()
        if token in "<(" : depth += 1
        elif token in ">)": depth -= 1
        elif token == "&&" and depth == 0:
            parts.append(text[start:match.start()].strip())
            start = match.end()
    if parts:
        parts.append(text[start:].strip())
        text = " && ".join(sorted(parts))
    return text


def canonical_template_names(text: str) -> str:
    names = re.findall(r"(?:class|typename)\s+([A-Za-z_]\w*)", text)
    for match in re.finditer(r"template\s*<(.+?)>", text, re.S):
        for parameter in split_top_level(match.group(1), ","):
            tail = re.search(r"([A-Za-z_]\w*)\s*(?:=.*)?$", parameter.strip())
            if tail:
                names.append(tail.group(1))
    for i, name in enumerate(dict.fromkeys(names)):
        text = re.sub(rf"\b{re.escape(name)}\b", f"T{i}", text)
    return text


def normalize_declaration(text: str) -> str:
    text = canonical_template_names(text)
    text = re.sub(r"\b(class|typename)\b", "typename", text)
    text = re.sub(r"\[\[[^]]+\]\]", "", text)
    text = re.sub(r"\s*=\s*(?:default|delete)\s*;?$", lambda m: m.group(), text)
    return normalize_space(text)


def balanced_block(text: str, start: int) -> str | None:
    brace = text.find("{", start)
    if brace < 0:
        return None
    depth = 0
    for i in range(brace, len(text)):
        if text[i] == "{": depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return text[start:i + 1]
    return None


def split_scoped_declarations(block: str) -> list[tuple[str, str]]:
    """Split synopsis declarations and retain their namespace/class scope."""
    out: list[tuple[str, str]] = []
    current: list[str] = []
    paren = angle = 0
    scopes = ["std"]
    brace_scopes: list[bool] = []
    for line in block.splitlines():
        line = re.sub(r"//.*", "", line).strip()
        if not line or line in {"public:", "private:", "protected:"}:
            continue
        for ch in line + " ":
            current.append(ch)
            if ch == "(": paren += 1
            elif ch == ")": paren = max(0, paren - 1)
            elif ch == "<": angle += 1
            elif ch == ">": angle = max(0, angle - 1)
            elif ch == "{" and paren == angle == 0:
                prefix = "".join(current[:-1]).strip()
                prefix_kind, prefix_name = declaration_name(prefix + ";")
                namespace_match = re.search(r"\bnamespace\s+([A-Za-z_]\w*)\s*$", prefix)
                if prefix_kind in {"class_template", "specialization"} and prefix_name:
                    out.append((prefix + ";", "::".join(scopes)))
                    scopes.append(prefix_name)
                    brace_scopes.append(True)
                elif namespace_match and namespace_match.group(1) != "std":
                    scopes.append(namespace_match.group(1))
                    brace_scopes.append(True)
                else:
                    brace_scopes.append(False)
                current.clear()
            elif ch == "}" and paren == angle == 0:
                current.clear()
                pushed = brace_scopes.pop() if brace_scopes else False
                if pushed and len(scopes) > 1:
                    scopes.pop()
            elif ch == ";" and paren == angle == 0:
                raw = "".join(current).strip()
                if raw and raw != ";":
                    out.append((raw, "::".join(scopes)))
                current.clear()
    return out


def split_declarations(block: str) -> list[str]:
    return [raw for raw, _ in split_scoped_declarations(block)]


def declaration_name(raw: str) -> tuple[str, str]:
    compact = normalize_space(raw)
    body = compact
    while body.startswith("template<"):
        depth = 0
        end = -1
        for i, ch in enumerate(body[len("template"):], len("template")):
            if ch == "<": depth += 1
            elif ch == ">":
                depth -= 1
                if depth == 0: end = i + 1; break
        if end < 0: break
        body = body[end:].strip()
    if re.search(r"\)\s*->", body) and "(" in body:
        before = body.split("(", 1)[0]
        return "deduction_guide", before.split()[-1]
    if re.search(r"\bconcept\s+\w+", body):
        return "concept", re.search(r"\bconcept\s+(\w+)", body).group(1)  # type: ignore[union-attr]
    if re.search(r"\busing\s+\w+\s*=", body):
        return "alias", re.search(r"\busing\s+(\w+)\s*=", body).group(1)  # type: ignore[union-attr]
    class_match = re.search(r"\b(class|struct)\s+([\w:]+)(?:\s*<[^;{]+>)?", body)
    if class_match:
        return ("specialization" if "<" in class_match.group(0) else "class_template"), class_match.group(2).split("::")[-1]
    var_match = re.search(r"\b(?:inline\s+)?(?:constexpr|constinit)\s+[^();=]+\s+(\w+)\s*(?:=|;)", body)
    if var_match:
        return "variable", var_match.group(1)
    ops = list(re.finditer(r"\boperator\s*(?:\[\]|\(\)|[^\s(]+)", body))
    if ops:
        return "function", re.sub(r"\s+", "", ops[-1].group())
    funcs = list(re.finditer(r"([~A-Za-z_]\w*)\s*\(", body))
    if funcs:
        name = funcs[0].group(1)
        return ("destructor" if name.startswith("~") else "function"), name
    return "unknown", ""


def section_anchor(text: str) -> str:
    match = re.search(r"\[([\w.-]+)\]", text[:500])
    return match.group(1) if match else ""


def extract_standard(root: Path, public_headers: set[str]) -> tuple[list[Declaration], list[Warning]]:
    manifest = json.loads((root / "manifest.json").read_text(encoding="utf-8"))
    entries = manifest["entries"]
    anchor_headers: dict[str, str] = {}
    entity_headers: dict[str, str] = {}
    parsed: list[tuple[dict[str, Any], str, str]] = []
    warnings: list[Warning] = []
    for entry in entries:
        title = entry["title"]
        if not ("synopsis" in title.lower() or "overview" in title.lower()):
            continue
        path = root / "sections" / entry["file"]
        parser = ClauseParser()
        try:
            parser.feed(path.read_text(encoding="utf-8"))
        except (OSError, UnicodeError) as exc:
            warnings.append(Warning("standard", "", entry["file"], str(exc)))
            continue
        text = clean_clause(parser.text)
        anchor = section_anchor(text)
        parsed.append((entry, text, anchor))
        header_match = re.search(r"Header\s+<([^>]+)>\s+synopsis", title, re.I)
        if header_match:
            header = header_match.group(1)
            if header in public_headers:
                if anchor:
                    anchor_headers[anchor.removesuffix(".syn")] = header
                for kind, name in map(declaration_name, split_declarations(text)):
                    if name:
                        entity_headers.setdefault(name.lower(), header)

    declarations: list[Declaration] = []
    for entry, text, anchor in parsed:
        title = entry["title"]
        header_match = re.search(r"Header\s+<([^>]+)>\s+synopsis", title, re.I)
        header = header_match.group(1) if header_match else ""
        if not header:
            prefixes = [key for key in anchor_headers if anchor == key or anchor.startswith(key + ".")]
            if prefixes:
                header = anchor_headers[max(prefixes, key=len)]
        if not header and "overview" in title.lower():
            # The immediately enclosing titled entity is normally present in the anchor.
            candidates = [part for part in anchor.split(".") if part.lower() in entity_headers]
            if candidates:
                header = entity_headers[candidates[0].lower()]
        if header not in public_headers:
            # <version> defines macros for itself and the headers named in comments.
            if not re.search(r"Header\s+<version>\s+synopsis", title, re.I):
                continue
        if re.search(r"Header\s+<version>\s+synopsis", title, re.I):
            for match in re.finditer(r"#define\s+(__cpp_lib_\w+)\s+(\d+L)(.*?)(?=#define|$)", text, re.S):
                name, value, following = match.groups()
                targets = {"version", *re.findall(r"<([^>]+)>", following)} & public_headers
                for target in targets:
                    declarations.append(Declaration(
                        "standard", target, "standard", "feature_macro", name, "", match.group(0)[:300],
                        value, value, location=entry["file"], section=anchor,
                    ))
        start = text.find("namespace std")
        if start < 0:
            continue
        block = balanced_block(text, start)
        if not block:
            warnings.append(Warning("standard", header, entry["file"], "unbalanced namespace synopsis"))
            continue
        for raw, scope in split_scoped_declarations(block):
            if "exposition only" in raw:
                continue
            kind, name = declaration_name(raw)
            if kind == "function" and scope.split("::")[-1] == name:
                kind = "constructor"
            if kind == "unknown" or not name:
                warnings.append(Warning("standard", header, entry["file"], f"unparsed declaration: {normalize_space(raw)[:180]}"))
                continue
            constraints = ""
            requires = re.search(r"\brequires\s+(.+?)(?=\b(?:class|struct|using|constexpr|inline|static)\b|$)", raw, re.S)
            if requires:
                constraints = normalize_constraint(requires.group(1))
            template = re.search(r"template\s*<(.+?)>", raw, re.S)
            params = template.group(1) if template else ""
            arity = len(split_top_level(params, ",")) if params else 0
            defaults = sum("=" in x for x in split_top_level(params, ",")) if params else 0
            qualifiers = qualifier_set(raw)
            norm = normalize_declaration(raw)
            declarations.append(Declaration(
                "standard", header, "standard", kind, name, scope, raw[:1000], norm,
                structural_signature(raw, kind, name, arity, norm), arity, defaults, constraints, qualifiers,
                entry["file"], anchor, "MEDIUM" if "implementation-defined" in raw or "see below" in raw else "HIGH",
            ))
    unique: dict[tuple[str, str, str, str, str], Declaration] = {}
    for declaration in declarations:
        unique[(declaration.header, declaration.kind, declaration.scope, declaration.name, declaration.signature)] = declaration
    return list(unique.values()), warnings


def split_top_level(text: str, separator: str) -> list[str]:
    if not text.strip(): return []
    parts, start, depth = [], 0, 0
    for i, ch in enumerate(text):
        if ch in "<({[": depth += 1
        elif ch in ">)}]": depth -= 1
        elif ch == separator and depth == 0:
            parts.append(text[start:i].strip()); start = i + 1
    parts.append(text[start:].strip())
    return parts


def qualifier_set(text: str) -> list[str]:
    paren = angle = 0
    for i, ch in enumerate(text):
        if ch == "(": paren += 1
        elif ch == ")": paren = max(0, paren - 1)
        elif ch == "<": angle += 1
        elif ch == ">": angle = max(0, angle - 1)
        elif ch == "{" and paren == angle == 0:
            text = text[:i]
            break
    out = []
    for q in ("static", "constexpr", "consteval", "explicit", "noexcept", "= delete", "= default"):
        if q in text: out.append(q.replace(" ", "_"))
    tail = text.rsplit(")", 1)[-1] if ")" in text else ""
    for q in ("const", "volatile", "&&", "&"):
        if re.search(rf"(?<!\w){re.escape(q)}(?!\w)", tail): out.append("ref_" + q if "&" in q else q)
    return sorted(set(out))


def signature_without_qualifiers(text: str) -> str:
    text = re.sub(r"\b(?:constexpr|consteval|explicit|static)\b", "", text)
    text = re.sub(r"\bnoexcept(?:\([^;]*\))?", "", text)
    text = re.sub(r"\brequires\b.*?(?=;|\{|$)", "", text)
    return normalize_space(text)


def structural_signature(raw: str, kind: str, name: str, template_arity: int, fallback: str = "") -> str:
    raw = canonical_template_names(raw)
    if kind in {"class_template", "specialization", "concept"}:
        return f"{kind}:{name}/{template_arity}"
    if kind in {"alias", "alias_template", "variable", "feature_macro"}:
        value = raw.split("=", 1)[1] if "=" in raw else fallback or raw
        return normalize_declaration(value).removesuffix(";")
    if kind not in {"function", "constructor", "destructor", "conversion", "deduction_guide"}:
        return normalize_declaration(fallback or raw)
    match = re.search(rf"{re.escape(name)}\s*\(", raw)
    if not match:
        return normalize_declaration(fallback or raw)
    start = match.end() - 1
    depth = 0
    end = -1
    for i in range(start, len(raw)):
        if raw[i] == "(": depth += 1
        elif raw[i] == ")":
            depth -= 1
            if depth == 0:
                end = i
                break
    if end < 0:
        return normalize_declaration(fallback or raw)
    params = []
    for param in split_top_level(raw[start + 1:end], ","):
        param = re.sub(r"\s*=.*$", "", param.strip())
        param = re.sub(r"(?<=[\s&*>])\b[A-Za-z_]\w*\s*$", "", param).strip()
        params.append(normalize_declaration(param))
    tail = raw[end + 1:].split("{", 1)[0]
    cvref = " ".join(q for q in ("const", "volatile", "&&", "&") if re.search(rf"(?<!\w){re.escape(q)}(?!\w)", tail))
    return normalize_space(f"{name}({','.join(params)}) {cvref}")


def signature_shape(signature: str) -> str:
    """Retain structural punctuation while ignoring unresolved identifier spelling."""
    return re.sub(r"\s+", "", re.sub(r"\b[A-Za-z_]\w*\b|\d+", "T", signature))


def signature_parameter_count(signature: str) -> int:
    start = signature.find("(")
    if start < 0: return -1
    depth = 0
    for i in range(start, len(signature)):
        if signature[i] == "(": depth += 1
        elif signature[i] == ")":
            depth -= 1
            if depth == 0:
                inside = signature[start + 1:i]
                return 0 if not inside.strip() else len(split_top_level(inside, ","))
    return -1


def ast_location(node: dict[str, Any], inherited: str = "") -> tuple[str, int]:
    loc = node.get("loc", {})
    file = loc.get("file") or loc.get("presumedFile") or inherited
    if not file:
        for key in ("spellingLoc", "expansionLoc"):
            nested = loc.get(key, {})
            file = nested.get("file") or nested.get("presumedFile") or file
    return str(file), int(loc.get("line") or loc.get("spellingLoc", {}).get("line") or 0)


@lru_cache(maxsize=512)
def read_source(file: str) -> str | None:
    try:
        return Path(file).read_text(encoding="utf-8", errors="replace")
    except OSError:
        return None


def source_text(node: dict[str, Any], inherited_file: str) -> str:
    begin = node.get("range", {}).get("begin", {})
    end = node.get("range", {}).get("end", {})
    file, _ = ast_location(node, inherited_file)
    text = read_source(os.path.abspath(file))
    if text is None: return ""
    a, b = begin.get("offset"), end.get("offset")
    if a is None or b is None: return ""
    return text[a:b + int(end.get("tokLen", 1))]


def extract_ftl_ast(ast: dict[str, Any], header: str, mode: str, include_root: Path) -> list[Declaration]:
    out: list[Declaration] = []
    root_norm = os.path.normcase(os.path.abspath(str(include_root))) + os.sep
    wrapper = read_source(str(include_root / header)) or ""
    implementation_roots = {header, f"__{header}", f"_{header}"}
    implementation_roots.update(re.findall(r"#include\s*<(?:ftl/)?((?:__|_)[^/>]+)/", wrapper))

    def belongs_to_header(file: str) -> bool:
        absolute = os.path.normcase(os.path.abspath(file))
        if not absolute.startswith(root_norm):
            return False
        first = absolute[len(root_norm):].split(os.sep, 1)[0]
        return first in implementation_roots

    def walk(node: dict[str, Any], scopes: tuple[str, ...] = (), inherited_file: str = "", templated: bool = False, in_function: bool = False) -> None:
        kind = node.get("kind", "")
        file, line = ast_location(node, inherited_file)
        inherited_file = file or inherited_file
        name = node.get("name", "")
        if kind in {"CXXConstructorDecl", "CXXDestructorDecl", "CXXDeductionGuideDecl"}:
            name = re.sub(r"<.*>$", "", name)
        new_scopes = scopes
        if kind == "NamespaceDecl" and name:
            new_scopes = scopes + (name,)
        elif kind in {"CXXRecordDecl", "ClassTemplateDecl", "ClassTemplateSpecializationDecl"} and name and (not scopes or scopes[-1] != name):
            new_scopes = scopes + (name,)
        is_public_file = bool(file) and os.path.normcase(os.path.abspath(file)).startswith(root_norm) and belongs_to_header(file)
        in_api = any(x in {"ftl", "std"} for x in scopes) and not any(x == "detail" or x.startswith("__") for x in scopes)
        if not line and file:
            loc = node.get("loc", {})
            offset = loc.get("offset") or loc.get("spellingLoc", {}).get("offset")
            content = read_source(os.path.abspath(file))
            if offset is not None and content is not None:
                line = content.count("\n", 0, int(offset)) + 1
        if kind in DECL_KINDS and name and line and not node.get("isImplicit") and is_public_file and in_api and not in_function:
            raw = source_text(node, inherited_file) or node.get("type", {}).get("qualType", name)
            type_info = node.get("type", {})
            canonical = type_info.get("desugaredQualType") or type_info.get("qualType") or raw
            norm = normalize_declaration(canonical)
            constraints = ""
            req = re.search(r"\brequires\s+(.+?)(?=\{|;|$)", raw, re.S)
            if req: constraints = normalize_constraint(req.group(1))
            params = [x for x in node.get("inner", []) if x.get("kind") in {"TemplateTypeParmDecl", "NonTypeTemplateParmDecl", "TemplateTemplateParmDecl"}]
            qualifiers = qualifier_set(raw)
            if node.get("constexpr"): qualifiers.append("constexpr")
            if node.get("consteval"): qualifiers.append("consteval")
            if node.get("storageClass") == "static": qualifiers.append("static")
            out.append(Declaration(
                "ftl", header, mode, DECL_KINDS[kind], name, "::".join(scopes), raw[:1000], norm,
                structural_signature(raw, DECL_KINDS[kind], name, len(params), norm), len(params), sum("defaultArg" in x for x in params),
                constraints, sorted(set(qualifiers)), f"{Path(file).as_posix()}:{line}", "", "HIGH",
            ))
        child_templated = templated or kind in AST_WRAPPERS
        child_in_function = in_function or kind in {"FunctionDecl", "CXXMethodDecl", "CXXConstructorDecl", "CXXDestructorDecl", "CXXConversionDecl", "CXXDeductionGuideDecl"}
        for child in node.get("inner", []):
            walk(child, new_scopes, inherited_file, child_templated, child_in_function)

    walk(ast)
    unique: dict[tuple[str, str, str, str], Declaration] = {}
    for declaration in out:
        unique[(declaration.location, declaration.kind, declaration.name, declaration.signature)] = declaration
    return list(unique.values())


def run_clang(clang: Path, repo: Path, header: str, mode: str) -> tuple[dict[str, Any] | None, str]:
    include = repo / "include"
    with tempfile.TemporaryDirectory(prefix="ftl-audit-") as tmp:
        source = Path(tmp) / "audit.cpp"
        source.write_text(f"#include <{'ftl/' if mode == 'normal' else ''}{header}>\n", encoding="utf-8")
        cmd = [str(clang), "-std=c++23", "-fsyntax-only", "-fno-color-diagnostics", "-x", "c++",
               "-I", str(include if mode == "normal" else include / "ftl"),
               "-Xclang", "-ast-dump=json", str(source)]
        if mode == "replace": cmd.insert(-1, "-DFTL_REPLACE_STL=1")
        ast_path = Path(tmp) / "ast.json"
        with ast_path.open("w", encoding="utf-8") as ast_file:
            proc = subprocess.run(cmd, stdout=ast_file, stderr=subprocess.PIPE, text=True, encoding="utf-8", errors="replace")
        if proc.returncode:
            return None, proc.stderr[-4000:]
        try:
            with ast_path.open(encoding="utf-8") as ast_file:
                return json.load(ast_file), proc.stderr[-4000:]
        except (json.JSONDecodeError, OSError) as exc:
            return None, f"invalid Clang JSON: {exc}"


def extract_macros(clang: Path, repo: Path, header: str, mode: str) -> tuple[list[Declaration], str]:
    include = repo / "include"
    cmd = [str(clang), "-std=c++23", "-dM", "-E", "-x", "c++", "-I",
           str(include if mode == "normal" else include / "ftl"), "-"]
    if mode == "replace": cmd.insert(-1, "-DFTL_REPLACE_STL=1")
    proc = subprocess.run(cmd, input=f"#include <{'ftl/' if mode == 'normal' else ''}{header}>\n",
                          stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, encoding="utf-8", errors="replace")
    out = []
    for line in proc.stdout.splitlines():
        match = re.match(r"#define\s+(__cpp_lib_\w+)\s+(.+)", line)
        if match:
            name, value = match.groups()
            out.append(Declaration("ftl", header, mode, "feature_macro", name, "", line,
                                   normalize_space(value), normalize_space(value)))
    return out, proc.stderr[-2000:] if proc.returncode else ""


def extract_one_worker(clang_text: str, repo_text: str, header: str, mode: str) -> tuple[list[Declaration], list[Warning]]:
    clang, repo = Path(clang_text), Path(repo_text)
    declarations, macro_error = extract_macros(clang, repo, header, mode)
    warnings: list[Warning] = []
    if macro_error:
        warnings.append(Warning("ftl", header, mode, macro_error))
    ast, error = run_clang(clang, repo, header, mode)
    if ast is None:
        warnings.append(Warning("ftl", header, mode, error))
    else:
        declarations.extend(extract_ftl_ast(ast, header, mode, repo / "include/ftl"))
    return declarations, warnings


def load_overrides(path: Path) -> list[dict[str, Any]]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, list): raise ValueError("overrides.json must contain an array")
    for i, item in enumerate(data):
        if not isinstance(item, dict) or not item.get("reason") or not item.get("category"):
            raise ValueError(f"override {i} requires category and reason")
    return data


def matching_override(issue: Issue, overrides: list[dict[str, Any]]) -> dict[str, Any] | None:
    for item in overrides:
        checks = {
            "header": issue.header,
            "category": issue.category,
            "mode": issue.mode,
            "name": (issue.standard or issue.ftl).name if (issue.standard or issue.ftl) else "",
        }
        if all(not item.get(k) or item[k] == v for k, v in checks.items()):
            return item
    return None


def compare(standard: list[Declaration], ftl: list[Declaration], modes: list[str], overrides: list[dict[str, Any]]) -> list[Issue]:
    issues: list[Issue] = []
    std_groups: dict[tuple[str, str, str, str], list[Declaration]] = defaultdict(list)
    ftl_groups: dict[tuple[str, str, str, str, str], list[Declaration]] = defaultdict(list)
    def relative_scope(scope: str) -> str:
        parts = scope.split("::")
        if parts and parts[0] in {"std", "ftl"}: parts.pop(0)
        return "::".join(parts)
    def mode_text(text: str) -> str:
        if text == "ftl":
            return "std"
        text = text.replace("ftl::", "std::")
        return re.sub(r"FTL_[A-Z0-9_]+_NAMESPACE", "std", text)
    for d in standard: std_groups[(d.header, d.kind, relative_scope(d.scope), d.name)].append(d)
    for d in ftl: ftl_groups[(d.mode, d.header, d.kind, relative_scope(d.scope), d.name)].append(d)
    for key, expected in std_groups.items():
        header, kind, scope, name = key
        for mode in modes:
            actual = ftl_groups.get((mode, header, kind, scope, name), [])
            if not actual:
                cpo = ftl_groups.get((mode, header, "variable", scope, name), []) if kind == "function" else []
                if cpo and scope.endswith("ranges"):
                    issues.append(Issue("MANUAL REVIEW", "LOW", header, "standard function set is implemented as a ranges function object; inspect call operators", expected[0], cpo[0], mode))
                    continue
                category = {
                    "feature_macro": "FEATURE MACRO MISMATCH",
                    "specialization": "SPECIALIZATION MISSING",
                    "deduction_guide": "DEDUCTION GUIDE MISSING",
                }.get(kind, "MISSING")
                issues.append(Issue(category, "HIGH", header, "declaration not found", expected[0], None, mode))
                continue
            if len(actual) != len(expected):
                issues.append(Issue("OVERLOAD COUNT MISMATCH", "HIGH", header,
                                    f"standard {len(expected)}, FTL {len(actual)}", expected[0], actual[0], mode))
            unmatched = actual.copy()
            possible_reported = False
            for want in expected:
                exact = next((x for x in unmatched if x.signature == want.signature), None)
                candidate = exact
                if candidate is None and unmatched:
                    same_arity = [x for x in unmatched if signature_parameter_count(x.signature) == signature_parameter_count(want.signature)]
                    pool = same_arity or unmatched
                    candidate = max(pool, key=lambda x: difflib.SequenceMatcher(None, want.signature, x.signature).ratio())
                if candidate:
                    unmatched.remove(candidate)
                    if want.constraints != candidate.constraints:
                        issues.append(Issue("CONSTRAINT MISMATCH", "MEDIUM", header, "declaration identity/arity match; constraints differ", want, candidate, mode))
                    if want.qualifiers != candidate.qualifiers:
                        diff = set(want.qualifiers) ^ set(candidate.qualifiers)
                        cat = "NOEXCEPT MISMATCH" if any("noexcept" in x for x in diff) else "CONSTEXPR MISMATCH" if any(x in {"constexpr", "consteval"} for x in diff) else "QUALIFIER MISMATCH"
                        issues.append(Issue(cat, "HIGH", header, f"qualifiers differ: {sorted(diff)}", want, candidate, mode))
                    if exact is None:
                        if kind == "feature_macro":
                            issues.append(Issue("FEATURE MACRO MISMATCH", "HIGH", header, f"standard value {want.signature}, FTL value {candidate.signature}", want, candidate, mode))
                            continue
                        if signature_shape(want.signature) != signature_shape(candidate.signature):
                            issues.append(Issue("SIGNATURE MISMATCH", "MEDIUM", header, "same declaration identity; structural type shape differs", want, candidate, mode))
                        elif not possible_reported:
                            issues.append(Issue("POSSIBLE EQUIVALENT SPELLING", "LOW", header, "identifier/type spelling differs after structural normalization", want, candidate, mode))
                            possible_reported = True
    standard_identities = {(d.header, d.kind, relative_scope(d.scope), d.name) for d in standard}
    standard_names = {(d.header, relative_scope(d.scope), d.name) for d in standard}
    seen_extra: set[tuple[str, str, str, str, str]] = set()
    for declaration in ftl:
        identity = (declaration.header, declaration.kind, relative_scope(declaration.scope), declaration.name)
        direct_marker = f"/include/ftl/{declaration.header}:"
        extra_key = (declaration.mode, *identity)
        name_identity = (declaration.header, relative_scope(declaration.scope), declaration.name)
        if (identity not in standard_identities and name_identity not in standard_names and
                declaration.kind != "feature_macro" and "{" not in declaration.raw and
                relative_scope(declaration.scope) in {"", "pmr", "ranges", "literals"} and
                direct_marker in declaration.location.replace("\\", "/") and extra_key not in seen_extra):
            issues.append(Issue("EXTRA", "LOW", declaration.header,
                                "directly declared in the public wrapper but absent from the extracted synopsis",
                                None, declaration, declaration.mode))
            seen_extra.add(extra_key)
    # Mode mismatch is useful even when the standard matching is incomplete.
    by_mode = {m: {(d.header, d.kind, mode_text(d.scope), mode_text(d.name), mode_text(d.signature)) for d in ftl if d.mode == m} for m in modes}
    mode_lookup = {(d.mode, d.header, d.kind, mode_text(d.scope), mode_text(d.name), mode_text(d.signature)): d for d in ftl}
    if len(modes) == 2:
        for item in sorted(by_mode[modes[0]] ^ by_mode[modes[1]]):
            header, kind, scope, name, _ = item
            present = modes[0] if item in by_mode[modes[0]] else modes[1]
            d = mode_lookup[(present, *item)]
            issues.append(Issue("MODE MISMATCH", "HIGH", header, f"present only in {present} mode", None, d, present))
    for issue in issues:
        issue.override = matching_override(issue, overrides)
    return issues


def serialize(obj: Any) -> Any:
    if dataclasses.is_dataclass(obj): return {k: serialize(v) for k, v in dataclasses.asdict(obj).items()}
    if isinstance(obj, list): return [serialize(x) for x in obj]
    if isinstance(obj, dict): return {k: serialize(v) for k, v in obj.items()}
    return obj


def write_reports(output: Path, headers: list[str], standard: list[Declaration], ftl: list[Declaration], issues: list[Issue], warnings: list[Warning], overrides: list[dict[str, Any]]) -> None:
    output.mkdir(parents=True, exist_ok=True)
    active = [x for x in issues if not x.override]
    modes = sorted({x.mode for x in ftl if x.mode in {"normal", "replace"}})
    def report_scope(scope: str) -> str:
        parts = scope.split("::")
        if parts and parts[0] in {"std", "ftl"}: parts.pop(0)
        return "::".join(parts)
    ftl_names = {(x.mode, x.header, x.kind, report_scope(x.scope), x.name) for x in ftl}
    matched = sum(
        any((mode, declaration.header, kind, report_scope(declaration.scope), declaration.name) in ftl_names
            for kind in ({declaration.kind, "variable"} if declaration.kind == "function" else {declaration.kind}))
        for declaration in standard for mode in modes
    )
    summary = {
        "headers_audited": len(headers), "standard_declarations": len(standard), "ftl_declarations": len(ftl),
        "standard_declarations_matched": matched,
        "issues": len(active), "overridden_findings": len(issues) - len(active), "parse_warnings": len(warnings),
        "categories": dict(sorted(Counter(x.category for x in active).items())),
    }
    payload = {"summary": summary, "issues": serialize(issues), "warnings": serialize(warnings),
               "overrides": overrides, "standard_model": serialize(standard), "ftl_model": serialize(ftl)}
    (output / "report.json").write_text(json.dumps(payload, indent=2), encoding="utf-8")
    lines = ["FTL C++23 / N4950 structural conformance audit", "=" * 49, "", "Summary"]
    lines += [f"  {k.replace('_', ' ').title()}: {v}" for k, v in summary.items() if k != "categories"]
    lines += [f"  {k}: {v}" for k, v in summary["categories"].items()]
    for header in headers:
        found = [x for x in issues if x.header == header]
        warns = [x for x in warnings if x.header == header]
        if not found and not warns: continue
        lines += ["", f"<{header}>", "-" * (len(header) + 2)]
        for issue in found:
            lines += ["", f"{issue.category} [{issue.confidence}] ({issue.mode})", f"  Result: {issue.message}"]
            if issue.standard:
                lines += [f"  Standard: [{issue.standard.section}] {issue.standard.raw[:500]}"]
            if issue.ftl:
                lines += [f"  FTL: {issue.ftl.location} {issue.ftl.raw[:500]}"]
            if issue.override:
                lines += [f"  OVERRIDE ({issue.override.get('status', 'accepted')}): {issue.override['reason']}"]
        for warning in warns:
            lines += ["", "PARSE WARNING [LOW]", f"  {warning.location}: {warning.message}"]
    general = [x for x in warnings if not x.header]
    if general:
        lines += ["", "General parse warnings", "----------------------"]
        lines += [f"  {x.location}: {x.message}" for x in general]
    (output / "report.txt").write_text("\n".join(lines) + "\n", encoding="utf-8")


def find_clang(explicit: str | None) -> Path:
    candidates = [explicit, os.environ.get("CLANG"),
                  r"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\x64\bin\clang.exe"]
    for value in candidates:
        if value and Path(value).is_file(): return Path(value)
    raise FileNotFoundError("clang not found; pass --clang")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument("--standard-root", type=Path, default=Path.home() / "Documents/CppStandards/ISO_IEC_14882_2024_html")
    parser.add_argument("--output", type=Path)
    parser.add_argument("--clang")
    parser.add_argument("--header", action="append", help="audit only this header (repeatable)")
    parser.add_argument("--mode", choices=("normal", "replace", "both"), default="both")
    parser.add_argument("--jobs", type=int, default=22)
    parser.add_argument("--ast-jobs", type=int, default=6, help="maximum memory-heavy Clang AST workers")
    args = parser.parse_args(argv)
    repo = args.repo.resolve()
    output = (args.output or repo / "build/standard-audit").resolve()
    public = sorted(p.name for p in (repo / "include/ftl").iterdir() if p.is_file() and not p.suffix)
    headers = args.header or public
    unknown = set(headers) - set(public)
    if unknown: parser.error(f"unknown public headers: {', '.join(sorted(unknown))}")
    modes = [args.mode] if args.mode != "both" else ["normal", "replace"]
    clang = find_clang(args.clang)
    overrides = load_overrides(Path(__file__).with_name("overrides.json"))
    standard, warnings = extract_standard(args.standard_root.resolve(), set(headers))
    ftl: list[Declaration] = []
    tasks = [(header, mode) for header in headers for mode in modes]
    with ProcessPoolExecutor(max_workers=max(1, min(args.jobs, args.ast_jobs))) as pool:
        futures = {pool.submit(extract_one_worker, str(clang), str(repo), header, mode): (header, mode) for header, mode in tasks}
        for future in as_completed(futures):
            header, mode = futures[future]
            try:
                declarations, local_warnings = future.result()
            except Exception as exc:  # keep a full audit actionable when one header fails
                warnings.append(Warning("ftl", header, mode, f"extractor failure: {exc}"))
                continue
            ftl.extend(declarations)
            warnings.extend(local_warnings)
    issues = compare(standard, ftl, modes, overrides)
    write_reports(output, headers, standard, ftl, issues, warnings, overrides)
    print(f"Wrote {output / 'report.txt'} and {output / 'report.json'}")
    print(f"Audited {len(headers)} headers; {len(standard)} standard declarations; {len(ftl)} FTL declarations; {len(issues)} findings; {len(warnings)} warnings")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
