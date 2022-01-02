# 2D Graphics Library

By Ross Smith

_[GitHub repository](https://github.com/CaptainCrowbar/rs-graphics-2d)_

## Overview

```c++
#include "rs-graphics-2d.hpp"
namespace RS::Graphics::Plane;
```

This library covers the 2D part of my graphics library.

The CMake file includes an `install` target to copy the headers into
`/usr/local/include` or the equivalent. Headers can be included individually
as required, or the entire library can be included using
`"rs-graphics-2d.hpp"`.

Other libraries required (these are all header-only):

* [My core graphics library](https://github.com/CaptainCrowbar/rs-graphics-core)
* [My formatting library](https://github.com/CaptainCrowbar/rs-format)
* [My unit test library](https://github.com/CaptainCrowbar/rs-unit-test)
  (not required unless you want to run the unit tests).

## Index

* [Version information](version.html)
* [Image](image.html)
