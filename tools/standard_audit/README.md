# FTL standard synopsis audit

`audit.py` compares declaration-sized extracts from the local N4950/ISO C++23
HTML against Clang ASTs for each public FTL header. The standard tree is only
read; reports contain section references and short declaration snippets, never
copied clauses.

```powershell
python tools/standard_audit/audit.py
python tools/standard_audit/audit.py --header vector
```

The scheduler accepts `--jobs 22` and uses a memory-bounded, work-stealing
process pool for AST extraction. `--ast-jobs` defaults to 6 because Clang JSON
trees are large; increase it only when memory measurements justify doing so.

The default reports are `build/standard-audit/report.txt` and `report.json`.
Use `--standard-root` or `--clang` when the local installations differ.

## Overrides

`overrides.json` is an array. Each entry requires `category` and `reason`, and
may select `header`, `name`, and `mode`. Optional `status` describes whether a
finding is a defect-report deviation, intentional behavior, parser limitation,
or accepted equivalent. Matching findings remain visible and are counted as
overridden rather than silently removed.

```json
[
  {
    "header": "optional",
    "name": "optional",
    "category": "EXTRA",
    "status": "intentional extension",
    "reason": "FTL intentionally provides optional<T&>."
  }
]
```
