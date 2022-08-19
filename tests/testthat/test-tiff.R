test_that("TIFF file signature", {
  skip_if_not("tiff" %in% ugd_renderers()$id, "TIFF renderer not installed")

  # TIFF has a different signature depending on endianness
  file_magic_le <- as.raw(c(0x49, 0x49, 0x2A, 0x00))
  file_magic_be <- as.raw(c(0x4D, 0x4D, 0x00, 0x2A))

  ugd_magic <- ugd_render_inline({
        plot(1)
    }, as = "tiff")[seq_along(file_magic_le)]

  expect_true(
    all.equal(file_magic_le, ugd_magic) ||
    all.equal(file_magic_be, ugd_magic)
  )
})
