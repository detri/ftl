# Third-party notices

## OpenLibm

FTL's math core adapts selected implementation files from OpenLibm v0.8.7
(https://github.com/JuliaMath/openlibm). Each adapted source retains its
applicable notice. The complete upstream aggregate notice, including the MIT,
ISC, FreeBSD, FDLIBM, and public-domain provenance, is preserved in
`external/openlibm/LICENSE.md`. No OpenLibm test code is imported.

The initial math core derives from FDLIBM sources carrying this notice:

> Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
>
> Developed at SunPro, a Sun Microsystems, Inc. business.
> Permission to use, copy, modify, and distribute this software is freely
> granted, provided that this notice is preserved.

OpenLibm integration work is also offered under the MIT License:

> Copyright (c) 2011-14 The Julia Project.
>
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in
> all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.

## Boost.Math

FTL's C++23 mathematical special functions adapt algorithms from Boost.Math
1.91.0 (https://github.com/boostorg/math). The adapted files identify their
upstream source and copyright holders. Boost.Math is used under the Boost
Software License 1.0, preserved in `external/boost_math/LICENSE.txt`.
