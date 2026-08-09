# FTL OpenLibm adaptation

This directory contains the permissively licensed implementation material
selected from OpenLibm v0.8.7. It is an internal FTL runtime component, not a
separately exposed copy of OpenLibm.

FTL's adaptation makes these deliberate changes:

- all exported and cross-file implementation symbols use the `ftl_olm_`
  prefix, preventing collisions with the platform C runtime;
- OpenLibm's floating-point-environment implementation and architecture
  assembly are excluded in favor of FTL's `<fenv.h>` layer and portable
  kernels;
- only x86-64 and AArch64 representation support is retained;
- LGPL test sources and nonstandard Bessel functions are not part of the FTL
  runtime;
- C99 complex kernels are retained as algorithm references and are adapted to
  FTL's C++ complex representation separately.

Original notices remain in every source file. `LICENSE.md` is the unmodified
OpenLibm v0.8.7 aggregate license notice.
