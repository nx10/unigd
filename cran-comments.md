## Test environments
- GitHub rlib/actions
- R-hub builder

## R CMD check results
There were no ERRORs or WARNINGs. 

> checking C++ specification ... NOTE
    Specified C++14: please drop specification unless essential

C++14 is essential for this package.

> checking installed package size ... NOTE
    installed size is  9.6Mb
    sub-directories of 1Mb or more:
      libs   8.4Mb

This package uses larger libraries.

## httpgd

We are splitting the `httpgd` package up into this package and the `httpgd` package.
This package has all the plotting and rendering code, and the `httpgd` package has the web-service code.

After/if this package is accepted to CRAN, we will submit the new version of the `httpgd` package to CRAN.

# I recently received the following email from CRAN (for httpgd):

> Prof Brian Ripley <ripley@stats.ox.ac.uk>
> Wed, Jan 10, 1:51 PM (12 days ago)
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

## Additional issues

> Please always write package names, software names and API (application
> programming interface) names in single quotes in title and description.
> e.g: --> 'C'
> Please note that package names are case sensitive.

'C' has been quoted in the description.

> If there are references describing the methods in your package, please
> add these in the description field of your DESCRIPTION file in the form
> authors (year) <doi:...>
> authors (year) <arXiv:...>
> authors (year, ISBN:...)
> or if those are not available: <https:...>
> with no space after 'doi:', 'arXiv:', 'https:' and angle brackets for
> auto-linking. (If you want to add a title as well please put it in
> quotes: "Title")

There are no references for this package.

> Please add \value to .Rd files regarding exported methods and explain
> the functions results in the documentation. Please write about the
> structure of the output (class) and also what the output means. (If a
> function does not return a value, please document that too, e.g.
> \value{No return value, called for side effects} or similar)
> Missing Rd-tags:
>       ugd_save_inline.Rd: \value
>       ugd_test_pattern.Rd: \value
> 
> \dontrun{} should only be used if the example really cannot be executed
> (e.g. because of missing additional software, missing API keys, ...) by
> the user. That's why wrapping examples in \dontrun{} adds the comment
> ("# Not run:") as a warning for the user.
> Please replace \dontrun with \donttest if possible.

These have been addressed.

Thank you!