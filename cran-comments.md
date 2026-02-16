## Resubmission

This package was archived on 2026-02-06 due to fmt 12.x compilation errors on
r-devel (Fedora). This release fixes those errors by upgrading the vendored fmt
library to 12.1.0 and resolving all related compiler warnings.

## Test environments

- GitHub Actions: macOS (release, oldrel), Windows (release), Ubuntu (devel,
  release, oldrel-1, oldrel-2)
- R-hub2

## R CMD check results

There were no ERRORs or WARNINGs.

> checking installed package size ... NOTE
    installed size is 15.0Mb
    sub-directories of 1Mb or more:
      libs  13.8Mb

This package contains compiled C++ code, which accounts for the larger
installed size.
