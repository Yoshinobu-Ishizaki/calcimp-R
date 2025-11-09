#' Calculate Input Impedance of Wind Instrument Tubes
#'
#' Calculate the input impedance of a wind instrument tube using acoustic
#' transmission theory. The tube geometry is specified in a mensur file
#' (either .men or .xmen format).
#'
#' @useDynLib calcimp, .registration = TRUE
#'
#' @param filename Character string. Path to the mensur file (.men or .xmen format)
#' @param max_freq Numeric. Maximum frequency in Hz (default: 2000.0)
#' @param step_freq Numeric. Frequency step in Hz (default: 2.5)
#' @param temperature Numeric. Temperature in Celsius (default: 24.0)
#' @param rad_calc Integer. Radiation impedance calculation mode:
#'   \itemize{
#'     \item 0 = NONE: No radiation impedance
#'     \item 1 = PIPE: Pipe radiation (default)
#'     \item 2 = BUFFLE: Baffled radiation
#'   }
#' @param dump_calc Logical. Enable wall damping calculation (default: TRUE)
#' @param sec_var_calc Logical. Enable section variation calculation (default: FALSE).
#'   This is experimental and may not be adequate.
#'
#' @return A data.frame with four columns:
#'   \describe{
#'     \item{freq}{Frequency in Hz}
#'     \item{real}{Real part of impedance}
#'     \item{imag}{Imaginary part of impedance}
#'     \item{mag}{Magnitude in dB (10*log10(real^2 + imag^2))}
#'   }
#'
#' @details
#' The function supports two mensur file formats:
#' \itemize{
#'   \item \strong{XMENSUR format (.xmen)}: Python-style format with # comments,
#'     brackets, and expressions. Recommended for new files.
#'   \item \strong{ZMENSUR format (.men)}: CSV-based format with % comments.
#'     Supported for backward compatibility.
#' }
#'
#' The file format is automatically detected from the file extension.
#'
#' @examples
#' \dontrun{
#' # Calculate impedance for a test instrument
#' result <- calcimp("sample/test.men")
#' plot(result$freq, result$mag, type="l", xlab="Frequency (Hz)", ylab="Magnitude (dB)")
#'
#' # With custom parameters
#' result <- calcimp("sample/trumpet.xmen",
#'                   max_freq = 3000,
#'                   step_freq = 5,
#'                   temperature = 20,
#'                   rad_calc = 1,
#'                   dump_calc = TRUE)
#' }
#'
#' @export
calcimp <- function(filename,
                    max_freq = 2000.0,
                    step_freq = 2.5,
                    temperature = 24.0,
                    rad_calc = 1L,
                    dump_calc = TRUE,
                    sec_var_calc = FALSE) {

  # Input validation
  if (!file.exists(filename)) {
    stop("File not found: ", filename)
  }

  if (!is.numeric(max_freq) || max_freq <= 0) {
    stop("max_freq must be a positive number")
  }

  if (!is.numeric(step_freq) || step_freq <= 0) {
    stop("step_freq must be a positive number")
  }

  if (!is.numeric(temperature)) {
    stop("temperature must be a number")
  }

  if (!is.numeric(rad_calc) || !(rad_calc %in% 0:2)) {
    stop("rad_calc must be 0 (NONE), 1 (PIPE), or 2 (BUFFLE)")
  }

  # Convert to integer for C
  rad_calc <- as.integer(rad_calc)

  # Call C function
  result <- .Call("r_calcimp",
                  filename,
                  max_freq,
                  step_freq,
                  temperature,
                  rad_calc,
                  dump_calc,
                  sec_var_calc)

  return(result)
}

#' Print Mensur Structure
#'
#' Read and display the structure of a mensur file. Returns a data frame
#' with the tube geometry segments.
#'
#' @param filename Character string. Path to the mensur file (.men or .xmen format)
#'
#' @return A data.frame with four columns:
#'   \describe{
#'     \item{df}{Front diameter in mm}
#'     \item{db}{Back diameter in mm}
#'     \item{r}{Length in mm}
#'     \item{comment}{Comment string for the segment}
#'   }
#'
#' @details
#' This function is useful for inspecting the tube geometry defined in a mensur
#' file. All dimensions are converted from the internal representation (meters)
#' to millimeters for easier interpretation.
#'
#' @examples
#' \dontrun{
#' # Display mensur structure
#' mensur <- print_men("sample/test.men")
#' print(mensur)
#' }
#'
#' @export
print_men <- function(filename) {
  # Input validation
  if (!file.exists(filename)) {
    stop("File not found: ", filename)
  }

  # Call C function
  result <- .Call("r_print_men",
                  filename)

  return(result)
}

#' Radiation Impedance Calculation Modes
#'
#' Constants for specifying radiation impedance calculation mode in calcimp()
#'
#' @format NULL
#' @export
NONE <- 0L

#' @rdname NONE
#' @format NULL
#' @export
PIPE <- 1L

#' @rdname NONE
#' @format NULL
#' @export
BUFFLE <- 2L
