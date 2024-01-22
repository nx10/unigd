# `unigd` <img src="man/figures/logo.png" align="right" width="25%"/>


<!-- badges: start -->
[![R-CMD-check](https://github.com/nx10/unigd/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/nx10/unigd/actions/workflows/R-CMD-check.yaml)
[![CRAN](https://www.r-pkg.org/badges/version/unigd)](https://CRAN.R-project.org/package=unigd)
![downloads](https://cranlogs.r-pkg.org/badges/grand-total/unigd)
[![Codecov test coverage](https://codecov.io/gh/nx10/unigd/branch/master/graph/badge.svg)](https://app.codecov.io/gh/nx10/unigd?branch=master)
<!-- badges: end -->

A unified R graphics backend. Render R graphics fast and easy to many common file formats.

This package provides a thread-safe interface to power interactive graphics clients such as [`httpgd`](https://github.com/nx10/httpgd).

## Features

* Fast plotting
* Plot resizing and history
* Render to various image formats (SVG, PNG, PDF, EPS, ...)
* In-memory access to rendered graphics
* Platform independent
* Stateless thread-safe C client API for package developers

## Installation

You can install the development version of `unigd` from GitHub with:

``` r
# install.packages("remotes")
remotes::install_github("nx10/unigd")
```

See [system requirements](https://nx10.github.io/unigd/articles/a00_installation.html#system-requirements) for troubleshooting.

## Getting started

See [the guide](https://nx10.github.io/unigd/articles/b00_guide.html) for more details.

## Contributions welcome!

`unigd` is mostly written in C++, but contributions to the tests (in R) or the documentation are also always welcome!

## About &amp; License

Depends on `cpp11` and `systemfonts`.
    
Much of the font handling and SVG rendering code is modified code from the excellent [`svglite`](<https://github.com/r-lib/svglite>) package.

This project is licensed GPL v2.0.

It includes parts of [`svglite`](<https://github.com/r-lib/svglite>) (GPL &geq; 2) and [`fmt`](https://github.com/fmtlib/fmt) (MIT).

Full copies of the license agreements used by these components are included in [`./inst/licenses`](https://github.com/nx10/unigd/tree/master/inst/licenses).
