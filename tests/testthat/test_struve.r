# tests/testthat/test-struve.R

test_that("struve1 function returns correct value for x = 0.059978", {
  # Test struve1(0.059978) should equal 0.000763201
  x <- 0.059978
  expected <- 0.000763201
  
  result <- struve1(x)
  
  # Use tolerance for floating point comparison
  # Using 1e-6 to match the tolerance in the configure script
  expect_equal(result, expected, tolerance = 1e-6)
})

test_that("struve1 returns numeric value", {
  # Test that struve1 returns a numeric result
  result <- struve1(0.059978)
  
  expect_type(result, "double")
  expect_length(result, 1)
})

test_that("struve1 works with different input values", {
  # Test with a few different values to ensure function is working
  # These are just sanity checks that the function executes

  result1 <- struve1(0.1)
  expect_equal(result1, 0.00212065, tolerance = 1e-6)

  result2 <- struve1(0.5)
  expect_equal(result2, 0.0521737, tolerance = 1e-6)

  result3 <- struve1(1.0)
  expect_equal(result3, 0.1984573362, tolerance = 1e-6)
})
