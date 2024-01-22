## Test environments
- GitHub rlib/actions
- R-hub builder

## R CMD check results
There were no ERRORs or WARNINGs. 

There was 1 NOTE:

* checking installed package size ... NOTE
    installed size is  5.6Mb
    sub-directories of 1Mb or more:
      libs   5.4Mb

## httpgd

We are splitting the `httpgd` package up into this package and the `httpgd` package.
This package has all the plotting and rendering code, and the `httpgd` package has the web-service code.

After/if this package is accepted to CRAN, we will submit the new version of the `httpgd` package to CRAN.

# I recently received the following email from CRAN (for httpgd):

> Prof Brian Ripley <ripley@stats.ox.ac.uk>
> Wed, Jan 10, 1:51 PM (12 days ago)
> to me, CRAN
> 
> Dear maintainer,
> 
> Please see the problems shown on
> <https://cran.r-project.org/web/checks/check_results_httpgd.html>.
> 
> Please correct before 2024-01-24 to safely retain your package on CRAN.
> 
> Do remember to look at the 'Additional issues'.
> 
> The CRAN Team

The clang 18 warnings have been addressed in this package.
