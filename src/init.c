/*
 * init.c
 * Registration of C routines for R package
 * Required for R CMD check compliance
 */

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

/* Declare C functions */
SEXP r_calcimp(SEXP filename_r, SEXP max_freq_r, SEXP step_freq_r,
               SEXP temperature_r, SEXP rad_calc_r, SEXP dump_calc_r,
               SEXP sec_var_calc_r);
SEXP r_print_men(SEXP filename_r);

/* Register C routines */
static const R_CallMethodDef CallEntries[] = {
    {"r_calcimp", (DL_FUNC) &r_calcimp, 7},
    {"r_print_men", (DL_FUNC) &r_print_men, 1},
    {NULL, NULL, 0}
};

void R_init_calcimp(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
