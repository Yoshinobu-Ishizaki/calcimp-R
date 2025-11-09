# Fix for struve() Function Returning Incorrect Values

## Problem
The `struve(1, x)` function was returning incorrect values (e.g., 0.003564 instead of 0.000763201 for x=0.059978), approximately 4.67× larger than expected.

## Root Cause
**Symbol collision**: The cephes library has its own `gamma()` function (the true Gamma function Γ(x)), but when the static library `libmd.a` was linked into the R package shared library, internal calls to `gamma()` from cephes functions like `struve()` were being resolved to the system math library's `gamma()` function instead, which is actually `lgamma` (ln(Γ(x)) - the natural logarithm of the Gamma function).

This caused all cephes calculations depending on the Gamma function to return completely incorrect values.

## Solution
Add the `-Wl,-Bsymbolic` linker flag when linking the shared library. This flag forces the linker to resolve internal symbol references within the shared library first before looking for external symbols.

## Changes Made
Modified `configure` script to add `-Wl,-Bsymbolic` to PKG_LIBS:

```bash
PKG_LIBS = $(shell pkg-config --libs glib-2.0 gsl 2>/dev/null || echo "-lglib-2.0 -lgsl -lgslcblas") -Wl,--whole-archive $(CEPHES_PATH)/libmd.a -Wl,--no-whole-archive -lm -Wl,-Bsymbolic
```

## Verification
```
struve(1.0, 0.059978) = 0.0007632005
Expected:               0.000763201
Difference:             4.5e-10 (within floating point precision)
```

## Key Insight
This was NOT an issue with static vs. shared library linking per se, but rather a symbol resolution order problem. The `-Wl,-Bsymbolic` flag ensures that cephes's internal function calls (like `struve()` calling `gamma()`) use cephes's own implementations rather than conflicting system library functions.
