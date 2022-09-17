test_that("Create pages", {
  ugd()
  pnum <- 10
  for (i in 1:pnum) {
    plot.new()
  }
  hs <- ugd_state()
  dev.off()
  expect_equal(hs$hsize, pnum)
})

test_that("Delete pages", {
  ugd()
  pnum <- 10
  dnum <- 3
  for (i in 1:pnum) {
    plot.new()
  }
  for (i in 1:dnum) {
    ugd_remove()
  }
  hs <- ugd_state()
  dev.off()
  expect_equal(hs$hsize, pnum - dnum)
})

test_that("Get page by index", {
  ugd()
  pnum <- 10
  dnum <- 3
  for (i in seq_len(pnum)) {
    plot.new()
    teststr <- paste0("123abc_plot_", i)
    text(0, 0, teststr)
  }
  json_out_4 <- ugd_render(page = 4, as = "json")
  json_out_4neg <- ugd_render(page = -4, as = "json")
  json_out_1 <- ugd_render(page = 1, as = "json")
  json_out_0 <- ugd_render(page = 0, as = "json")
  json_out_10 <- ugd_render(page = 10, as = "json")
  json_out_9neg <- ugd_render(page = -9, as = "json")
  expect_error(ugd_render(page = 11, as = "json"))
  expect_error(ugd_render(page = -10, as = "json"))
  dev.off()
  expect_true(grepl("123abc_plot_4", json_out_4, fixed = TRUE))
  expect_true(grepl("123abc_plot_6", json_out_4neg, fixed = TRUE))
  expect_true(grepl("123abc_plot_10", json_out_0, fixed = TRUE))
  expect_true(grepl("123abc_plot_1", json_out_1, fixed = TRUE))
  expect_true(grepl("123abc_plot_10", json_out_10, fixed = TRUE))
  expect_true(grepl("123abc_plot_1", json_out_9neg, fixed = TRUE))
})

test_that("Delete page by index", {
  ugd()
  pnum <- 10
  dnum <- 3
  for (i in 1:pnum) {
    plot.new()
    teststr <- paste0("123abc_plot_", i)
    text(0, 0, teststr)
  }
  ugd_remove(page = 4)
  hs <- ugd_state()
  svgs <- rep(NA, hs$hsize)
  for (i in 1:hs$hsize) {
    svgs[i] <- ugd_render(page = i, as = "json")
  }
  dev.off()
  expect_true(grepl("123abc_plot_3", svgs[3], fixed = TRUE))
  expect_true(grepl("123abc_plot_5", svgs[4], fixed = TRUE))
})

test_that("Clear pages", {
  ugd()
  pnum <- 10
  for (i in 1:pnum) {
    plot.new()
  }
  ugd_clear()
  hs <- ugd_state()
  dev.off()
  expect_equal(hs$hsize, 0)
})
