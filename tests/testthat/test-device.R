test_that("Device capabilities are reported", {
  ugd()
  expect_error(dev.capabilities(), regexp = NA) # Expect no error
  dev.off()
})
