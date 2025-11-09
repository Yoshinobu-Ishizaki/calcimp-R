# calcimp-R

R package for calculating input impedance of wind instrument tubes using acoustic transmission theory.

管楽器の入力インピーダンスを音響伝送理論を用いて計算するRパッケージです。

## System Requirements

### Required Libraries

**Required for installation and runtime:**

```bash
# Ubuntu/Debian
sudo apt-get install libglib2.0-dev libgsl-dev

# macOS
brew install glib gsl

# Windows (MSYS2)
pacman -S mingw-w64-x86_64-glib2 mingw-w64-x86_64-gsl
```

### External Library - Cephes (Automatically Downloaded)

- **Cephes Math Library** (`libmd.a`) - Required for Bessel and Struve functions
  - **Automatically downloaded and built** during package installation
  - Source: https://github.com/Yoshinobu-Ishizaki/cephes-lib
  - The installation process will handle this for you!

**Build Dependencies (needed for auto-download):**

```bash
# Ubuntu/Debian - Required for building cephes-lib
sudo apt-get install git autoconf automake libtool make gcc

# macOS - Usually already installed with Xcode Command Line Tools
xcode-select --install
```

**Optional: Using Pre-built Cephes Library**

If you already have cephes-lib built elsewhere, you can skip the auto-download:

```bash
# Set CEPHES_PATH to your existing Cephes library location
export CEPHES_PATH=/path/to/your/cephes-lib

# Then install
R CMD INSTALL .
```

## Installation

### Option 1: Install from GitHub (Recommended)

The cephes library will be **automatically downloaded and built** during installation!

```r
# Install devtools if not already installed
install.packages("devtools")

# Install calcimp from GitHub (cephes will be auto-downloaded)
devtools::install_github("Yoshinobu-Ishizaki/calcimp-R")
```

**Prerequisites:**
- System libraries (glib2, gsl) must be installed
- Build tools (git, autoconf, automake, libtool, make, gcc) for auto-building cephes

### Option 2: Install from Local Source

```bash
# Clone the repository
git clone https://github.com/Yoshinobu-Ishizaki/calcimp-R.git
cd calcimp-R

# The configure script will auto-download cephes-lib
./configure

# Install in R
R CMD INSTALL .
# Or use devtools in R:
# devtools::install()
```

### Option 3: Development Installation

For active development:

```bash
# Run configure to set up cephes-lib
./configure

# Then in R:
devtools::load_all()
```

## Usage Example

```r
library(calcimp)

# Calculate impedance for a tube geometry
result <- calcimp(
    "sample/test.men",      # Mensur file (tube geometry)
    max_freq = 2000.0,      # Maximum frequency in Hz
    step_freq = 2.5,        # Frequency step in Hz
    temperature = 24.0,     # Temperature in Celsius
    rad_calc = PIPE,        # Radiation mode: NONE, PIPE, or BUFFLE
    dump_calc = TRUE,       # Include damping on the wall
    sec_var_calc = FALSE    # Include effect by varying section area (experimental)
)

# Result is a data frame with columns: freq, real, imag, mag
head(result)

# Plot magnitude spectrum
plot(result$freq, result$mag, type="l",
     xlab="Frequency (Hz)", ylab="Magnitude (dB)",
     main="Input Impedance Spectrum")

# Inspect tube geometry
mensur <- print_men("sample/test.men")
print(mensur)
```

### Parameters

All parameters except `filename` are optional:

- `filename`: Path to mensur file (.men or .xmen format) - **Required**
- `max_freq`: Maximum frequency in Hz (default: 2000.0)
- `step_freq`: Frequency step in Hz (default: 2.5)
- `temperature`: Temperature in Celsius (default: 24.0)
- `rad_calc`: Radiation impedance mode (default: PIPE)
  - `NONE` (0): No radiation impedance
  - `PIPE` (1): Pipe radiation
  - `BUFFLE` (2): Baffled radiation
- `dump_calc`: Enable wall damping (default: TRUE)
- `sec_var_calc`: Enable section variation calculation (default: FALSE, experimental)

## Mensur File Formats

calcimp supports two file formats for describing tube geometry:

### 1. XMENSUR Format (.xmen)

Recommended format with Python-style syntax:

```
# Trumpet with valve
bore_dia = 11.5  # Variable definition

[  # Main bore
    10, bore_dia, 100,  # Can use expressions
    >, VALVE1, 1,       # Valve branch
    bore_dia, bore_dia, 150,
    <, VALVE1, 1,       # Valve merge
    OPEN_END
]

{, VALVE1  # Valve slide definition
    bore_dia, bore_dia, 300,
    OPEN_END
}
```

**Features:**
- Uses `#` for comments
- Supports arithmetic expressions and variables
- Keywords: `OPEN_END`, `CLOSED_END`, `MAIN`, `END_MAIN`, `GROUP`, `END_GROUP`, `BRANCH`, `MERGE`, `SPLIT`, `INSERT`
- Aliases: `>` = BRANCH, `<` = MERGE, `|` = SPLIT, `@` = INSERT

See [doc/xmensur.md](doc/xmensur.md) for complete specification.

### 2. ZMENSUR Format (.men)

Legacy CSV-based format (backward compatibility):

```
# Basic tube
10,10,1000,     % Cylindrical tube: 10mm diameter, 1000mm length
10,0,0,         % Open end terminator
```

See [doc/zmensur.md](doc/zmensur.md) for complete specification.

Both formats work seamlessly - the format is automatically detected from file extension.

## Development

### Build and Test Commands

```r
# Load package for interactive testing
devtools::load_all()

# Generate documentation from roxygen comments
devtools::document()

# Run tests (if available)
devtools::test()

# Check package (comprehensive checks)
devtools::check()

# Install locally
devtools::install()

# Clean compiled code
devtools::clean_dll()
```

### Important Notes

- Always register C functions in `src/init.c` - required for R CMD check
- Use roxygen2 comments (`#'`) for documentation - they auto-generate .Rd files
- Run `devtools::document()` after changing roxygen comments
- Run `devtools::check()` before committing - catches most issues
- **Memory management in C**: Always `PROTECT()` and `UNPROTECT()` R objects you create
- Keep C code portable: Avoid platform-specific code unless necessary
- Test edge cases: Empty files, invalid inputs, large data, etc.

### Setting Custom Cephes Path

If your cephes library is in a non-standard location:

```bash
# Set environment variable before installing
export CEPHES_PATH=/path/to/your/cephes-lib
R CMD INSTALL .
```

Or in your `~/.Renviron`:

```
CEPHES_PATH=/path/to/your/cephes-lib
```

## Troubleshooting

### Error: "Cephes library not found"

**Solution:** Build cephes library or set `CEPHES_PATH`:

```bash
# Build cephes library (see above)
# Then set path
export CEPHES_PATH=/path/to/cephes-lib
```

### Error: "gsl not found" or "glib not found"

**Solution:** Install system dependencies:

```bash
# Ubuntu/Debian
sudo apt-get install libglib2.0-dev libgsl-dev

# macOS
brew install glib gsl
```

### Compilation errors

**Solution:** Clean and rebuild:

```r
devtools::clean_dll()
devtools::load_all()
```

## License

MIT License

Original code by Yoshinobu Ishizaki (1999)

## Acknowledgements

This project incorporates the following third-party libraries:

- **Cephes Mathematical Library** by Stephen L. Moshier
  - Special mathematical functions (Bessel, Struve, Airy functions)
  - Copyright (C) 1995 Stephen L. Moshier
  - http://www.netlib.org/cephes/

- **TinyExpr** by Lewis Van Winkle
  - Expression parser for XMENSUR files
  - Copyright (c) 2015-2020 Lewis Van Winkle
  - https://github.com/codeplea/tinyexpr
  - Licensed under the zlib license

## References

- Fletcher & Rossing: "The Physics of Musical Instruments"

## Related Projects

- **calcimp-python**: Python version of this package
  - https://github.com/Yoshinobu-Ishizaki/calcimp-python
