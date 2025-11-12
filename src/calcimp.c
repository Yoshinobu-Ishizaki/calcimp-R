/*
 * calcimp_r.c
 * R interface for calcimp - calculate input impedance of given mensur
 * Adapted from Python version
 * 1999.05.03 - Yoshinobu Ishizaki
 */

#include <R.h>
#include <Rinternals.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <math.h>
#include <glib.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include "cephes.h"
#include "kutils.h"
#include "zmensur.h"
#include "xmensur.h"
#include "calcimp.h"
#include "acoustic_constants.h"

/**
 * R interface for calcimp
 *
 * @param filename_r R character string: path to mensur file
 * @param max_freq_r R numeric: maximum frequency in Hz
 * @param step_freq_r R numeric: frequency step in Hz
 * @param temperature_r R numeric: temperature in Celsius
 * @param rad_calc_r R integer: radiation calculation mode (0=NONE, 1=PIPE, 2=BUFFLE)
 * @param dump_calc_r R logical: enable wall damping
 * @param sec_var_calc_r R logical: enable section variation calculation
 * @return R data.frame with columns: freq, real, imag, mag
 */
SEXP r_calcimp(SEXP filename_r, SEXP max_freq_r, SEXP step_freq_r,
               SEXP temperature_r, SEXP rad_calc_r, SEXP dump_calc_r,
               SEXP sec_var_calc_r) {

    /* Convert R arguments to C types */
    const char* filename = CHAR(STRING_ELT(filename_r, 0));
    double max_freq = asReal(max_freq_r);
    double step_freq = asReal(step_freq_r);
    double temperature = asReal(temperature_r);
    int rad_calc = asInteger(rad_calc_r);
    int dump_calc = asLogical(dump_calc_r) ? WALL : NONE;
    int sec_var_calc = asLogical(sec_var_calc_r);

    /* Variables for computation */
    mensur *mensur_data;
    double complex *imp;
    int n_imp;
    double frq, mag, S;
    int i;
    acoustic_constants ac;

    /* Initialize acoustic constants based on temperature */
    init_acoustic_constants(&ac, temperature);

    /* Set configuration flags */
    ac.rad_calc = rad_calc;
    ac.dump_calc = dump_calc;
    ac.sec_var_calc = sec_var_calc;

    /* Read mensur file - detect format by extension */
    const char *ext = strrchr(filename, '.');
    if (ext != NULL && strcmp(ext, ".xmen") == 0) {
        /* XMENSUR format */
        mensur_data = read_xmensur(filename);
    } else {
        /* ZMENSUR format (default) */
        mensur_data = read_mensur(filename);
    }

    if (mensur_data == NULL) {
        error("Failed to read mensur file: %s", filename);
    }

    /* Calculate number of points */
    n_imp = (int)(max_freq / step_freq) + 1;

    /* Allocate memory for impedance calculations */
    imp = (double complex*)calloc(n_imp, sizeof(double complex));
    if (imp == NULL) {
        error("Failed to allocate memory for impedance calculation");
    }

    /* Get initial cross-sectional area */
    S = M_PI * pow(get_first_men(mensur_data)->df, 2) / 4;

    /* Calculate impedance */
    for (i = 0; i < n_imp; i++) {
        frq = i * step_freq;
        if (i == 0) {
            imp[i] = 0.0;
        } else {
            input_impedance(frq, mensur_data, 1, &imp[i], &ac);
            imp[i] *= S;  /* Convert to acoustic impedance density */
        }
    }

    /* Create R vectors for output */
    SEXP freq_vec = PROTECT(allocVector(REALSXP, n_imp));
    SEXP real_vec = PROTECT(allocVector(REALSXP, n_imp));
    SEXP imag_vec = PROTECT(allocVector(REALSXP, n_imp));
    SEXP mag_vec = PROTECT(allocVector(REALSXP, n_imp));

    double *freq_data = REAL(freq_vec);
    double *real_data = REAL(real_vec);
    double *imag_data = REAL(imag_vec);
    double *mag_data = REAL(mag_vec);

    /* Fill vectors with data */
    for (i = 0; i < n_imp; i++) {
        freq_data[i] = i * step_freq;
        real_data[i] = creal(imp[i]);
        imag_data[i] = cimag(imp[i]);
        mag = real_data[i] * real_data[i] + imag_data[i] * imag_data[i];
        mag_data[i] = (mag > 0) ? 10 * log10(mag) : mag;
    }

    free(imp);

    /* Create data frame */
    SEXP df = PROTECT(allocVector(VECSXP, 4));
    SET_VECTOR_ELT(df, 0, freq_vec);
    SET_VECTOR_ELT(df, 1, real_vec);
    SET_VECTOR_ELT(df, 2, imag_vec);
    SET_VECTOR_ELT(df, 3, mag_vec);

    /* Set column names */
    SEXP col_names = PROTECT(allocVector(STRSXP, 4));
    SET_STRING_ELT(col_names, 0, mkChar("freq"));
    SET_STRING_ELT(col_names, 1, mkChar("real"));
    SET_STRING_ELT(col_names, 2, mkChar("imag"));
    SET_STRING_ELT(col_names, 3, mkChar("mag"));
    setAttrib(df, R_NamesSymbol, col_names);

    /* Set class to data.frame */
    SEXP row_names = PROTECT(allocVector(INTSXP, 2));
    INTEGER(row_names)[0] = NA_INTEGER;
    INTEGER(row_names)[1] = -n_imp;
    setAttrib(df, R_RowNamesSymbol, row_names);

    SEXP df_class = PROTECT(allocVector(STRSXP, 1));
    SET_STRING_ELT(df_class, 0, mkChar("data.frame"));
    setAttrib(df, R_ClassSymbol, df_class);

    UNPROTECT(8);  /* freq_vec, real_vec, imag_vec, mag_vec, df, col_names, row_names, df_class */
    return df;
}

/**
 * R interface for print_men
 *
 * @param filename_r R character string: path to mensur file
 * @return R data.frame with columns: df, db, r, comment (dimensions in mm)
 */
SEXP r_print_men(SEXP filename_r) {
    const char* filename = CHAR(STRING_ELT(filename_r, 0));

    /* Read mensur file - detect format by extension */
    mensur *mensur_data;
    const char *ext = strrchr(filename, '.');
    if (ext != NULL && strcmp(ext, ".xmen") == 0) {
        /* XMENSUR format */
        mensur_data = read_xmensur(filename);
        if (mensur_data == NULL) {
            error("Failed to read XMENSUR file: %s", filename);
        }
    } else {
        /* ZMENSUR format */
        mensur_data = read_mensur(filename);
        if (mensur_data == NULL) {
            error("Failed to read ZMENSUR file: %s", filename);
        }
    }

    /* Count number of mensur elements */
    int n_elements = 0;
    mensur *m = get_first_men(mensur_data);
    while (m != NULL) {
        n_elements++;
        m = m->next;
    }

    /* Create R vectors for output */
    SEXP df_vec = PROTECT(allocVector(REALSXP, n_elements));
    SEXP db_vec = PROTECT(allocVector(REALSXP, n_elements));
    SEXP r_vec = PROTECT(allocVector(REALSXP, n_elements));
    SEXP comment_vec = PROTECT(allocVector(STRSXP, n_elements));

    double *df_data = REAL(df_vec);
    double *db_data = REAL(db_vec);
    double *r_data = REAL(r_vec);

    /* Fill vectors with data (convert from meters to mm) */
    m = get_first_men(mensur_data);
    int i = 0;
    while (m != NULL) {
        df_data[i] = m->df * 1000.0;  /* meters to mm */
        db_data[i] = m->db * 1000.0;
        r_data[i] = m->r * 1000.0;
        SET_STRING_ELT(comment_vec, i, mkChar(m->comment ? m->comment : ""));
        i++;
        m = m->next;
    }

    /* Create data frame */
    SEXP df = PROTECT(allocVector(VECSXP, 4));
    SET_VECTOR_ELT(df, 0, df_vec);
    SET_VECTOR_ELT(df, 1, db_vec);
    SET_VECTOR_ELT(df, 2, r_vec);
    SET_VECTOR_ELT(df, 3, comment_vec);

    /* Set column names */
    SEXP col_names = PROTECT(allocVector(STRSXP, 4));
    SET_STRING_ELT(col_names, 0, mkChar("df"));
    SET_STRING_ELT(col_names, 1, mkChar("db"));
    SET_STRING_ELT(col_names, 2, mkChar("r"));
    SET_STRING_ELT(col_names, 3, mkChar("comment"));
    setAttrib(df, R_NamesSymbol, col_names);

    /* Set class to data.frame */
    SEXP row_names = PROTECT(allocVector(INTSXP, 2));
    INTEGER(row_names)[0] = NA_INTEGER;
    INTEGER(row_names)[1] = -n_elements;
    setAttrib(df, R_RowNamesSymbol, row_names);

    SEXP df_class = PROTECT(allocVector(STRSXP, 1));
    SET_STRING_ELT(df_class, 0, mkChar("data.frame"));
    setAttrib(df, R_ClassSymbol, df_class);

    UNPROTECT(8);  /* df_vec, db_vec, r_vec, comment_vec, df, col_names, row_names, df_class */
    return df;
}

/**
 * R binding for struve function with v=1
 * 
 * @param x_sexp R numeric vector (only first element is used)
 * @return R numeric value - result of struve(1, x)
 */
SEXP r_struve1(SEXP x_sexp) {
  // Check input is numeric
  if (!isReal(x_sexp) && !isInteger(x_sexp)) {
    error("x must be numeric");
  }
  
  // Convert to double
  double x = asReal(x_sexp);
  
  // Check for invalid input
  if (!R_finite(x)) {
    error("x must be finite");
  }
  
  // Call cephes struve function with v=1
  double result = struve(1.0, x);
  
  // Return result as R numeric
  return ScalarReal(result);
}

