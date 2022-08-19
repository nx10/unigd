test_that("PNG file signature", {
  skip_if_not("png" %in% ugd_renderers()$id, "PNG renderer not installed")

  png_magic <- as.raw(c(0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A))
  ugd_magic <- ugd_render_inline({
        plot(1)
    }, as = "png")[seq_along(png_magic)]

  expect_equal(png_magic, ugd_magic)
})
