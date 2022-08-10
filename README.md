# `unigd`

<!-- badges: start -->
[![R-CMD-check](https://github.com/nx10/unigd/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/nx10/unigd/actions/workflows/R-CMD-check.yaml)
[![Codecov test coverage](https://codecov.io/gh/nx10/unigd/branch/master/graph/badge.svg)](https://app.codecov.io/gh/nx10/unigd?branch=master)
<!-- badges: end -->

A unified R graphics backend. Render R graphics fast and easy to any file format.

This package provides a thread-safe interface to power interactive graphics clients such as [`httpgd`](https://github.com/nx10/httpgd).

## Quick start

```R
library(unigd)

ugd() # Start a unigd device

plot(1, main="Anything") # Plot something

ugd_save("my_plot.svg") # Save a SVG...
ugd_save("my_plot.png") # ...and a PNG.

cat(ugd_render(as="svg")) # Or print the SVG (without ever creating a file)

dev.off() # Close the device
```

Helper functions make your code concise:

```R
ugd_save_inline({
    plot(1, main="Anything")
}, "my_plot.png")
```

## Installation

You can install the development version of unigd from GitHub with:

``` r
# install.packages("remotes")
remotes::install_github("nx10/unigd")
```

