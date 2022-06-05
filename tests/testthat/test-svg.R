test_that("SVG contains plot text", {
  ugd()
  plot.new()
  teststr <- "Some text abc123"
  text(0, 0, teststr)
  svg <- ugd_render()
  dev.off()
  expect_true(grepl(teststr, svg, fixed = TRUE))
})

test_that("boxplot returns valid SVG", {
  expect_warning(xmlSVG({
    boxplot(rnorm(10))
  }), regexp = NA)
})

#test_that("Append CSS with extra_css", {
#  testcss <- ".httpgd polyline { stroke: green; }"
#  ugd(webserver=F, extra_css = testcss)
#  plot(1)
#  svg <- ugd_render()
#  dev.off()
#  expect_true(grepl(testcss, svg, fixed = TRUE))
#})