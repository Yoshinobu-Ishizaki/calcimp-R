# CLAUDE.md

## Project Overview

This is an R package that uses C code to speed up computation. The package is designed to be installable via devtools::install_github() and will be hosted on GitHub.

## C source

Initially copied from "https://github.com/Yoshinobu-Ishizaki/calcimp-python"

Arange these as C bindings for R.

The function `calcimp` should return dataframe that consists of 4 columns -- frequency, real part, imaginary part, and magnitude.

## Package structure

- files in `src` are C implementations
- files in `R` are R codes
- tests folder are for unit test
- sample contains sample data for calcimp function



## Build and Test Commands

```
# Load package for interactive testing
devtools::load_all()

# Generate documentation from roxygen comments
devtools::document()

# Run tests
devtools::test()

# Check package (comprehensive checks)
devtools::check()

# Install locally
devtools::install()

# Clean compiled code
devtools::clean_dll()
```

## Important Notes for Claude Code

- Always register C functions in src/init.c - this is required for R CMD check
- Use roxygen2 comments (#') for documentation - they auto-generate .Rd files
- Run devtools::document() after changing roxygen comments
- Run devtools::check() before committing - it catches most issues
- Memory management in C: Always PROTECT() and UNPROTECT() R objects you create
- Keep C code portable: Avoid platform-specific code unless absolutely necessary
- Test edge cases: Empty vectors, NA values, large data, etc.

## Checklist Before Release

- [ ] All tests pass (devtools::test())
- [ ] Package checks clean (devtools::check())
- [ ] Documentation complete and accurate
- [ ] README has installation instructions
- [ ] LICENSE file included
- [ ] GitHub Actions passing (if configured)
- [ ] Version number updated in DESCRIPTION
- [ ] NEWS.md updated with changes (optional but recommended)

