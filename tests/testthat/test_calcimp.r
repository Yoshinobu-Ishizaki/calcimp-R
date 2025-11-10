# tests/testthat/test-calcimp.R

test_that("check calcimp produces correct output for testfile.txt", {
  # Path to test files (assuming they're in tests/testthat/)
  test_input <- "test.men"
  expected_output_file <- "test.imp"
  
  # Run calcimp
  out <- calcimp(test_input)
  
  # Read expected output
  expected <- read.csv(expected_output_file)
  names(expected) <- names(out) # ignore column name difference
  
  # Compare outputs
  # If out is a character vector:
  expect_equal(out, expected)
  
  # Alternative: If out is a single string, you might need:
  # expect_equal(out, paste(expected, collapse = "\n"))
})

test_that("calcimp handles missing input file gracefully", {
  # Test error handling
  expect_error(calcimp("nonexistent_file.txt"))
})
