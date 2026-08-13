# Security Policy

FTL is an experimental C++ standard-library replacement and is not currently
intended to provide a hardened security boundary.

Security issues are still taken seriously, particularly defects involving:

- memory safety
- allocator or object-lifetime violations
- undefined behavior reachable through valid public API usage
- filesystem path handling
- parsing of untrusted input
- format or regex processing
- concurrency and synchronization
- accidental hosted-library or ABI boundary violations

## Reporting a vulnerability

Please do not publicly disclose a vulnerability before it has had a reasonable
opportunity to be investigated.

Prefer GitHub's private vulnerability reporting or Security Advisory mechanism
for this repository when available.

If no private reporting mechanism is available, open a minimal GitHub issue
requesting a private contact method. Do not include exploit details, proof of
concepts, sensitive data, or other information that would make the
vulnerability easier to abuse in that public issue.

A useful report should include:

- the affected FTL revision or version
- compiler, compiler version, operating system, and architecture
- whether normal mode or `FTL_REPLACE_STL` is affected
- the affected header or facility
- a minimal reproducer when practical
- the expected and observed behavior
- the security impact
- any known workarounds

## Supported versions

FTL is currently developed from its main development line and does not yet
maintain long-lived security-supported release branches.

Security fixes are therefore made against the current supported codebase.
Older revisions may require updating to receive a fix.

## Scope

A standards-conformance defect is not automatically a security vulnerability.

Please use the normal issue tracker for ordinary conformance bugs, incorrect
constraints, missing overloads, portability problems, performance issues, and
other defects that do not have a meaningful security impact.

Platform runtime and ABI boundaries are part of FTL's threat surface when FTL
itself handles or exposes them. Vulnerabilities wholly inside an operating
system, compiler, C runtime, or other external dependency should normally be
reported to that project's maintainers instead.

## Disclosure

Once a vulnerability has been fixed, relevant details may be published so
users can understand the affected versions, impact, and remediation.

Credit will be given to reporters who want it.
