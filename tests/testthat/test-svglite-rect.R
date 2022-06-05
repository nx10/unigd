# https://github.com/r-lib/svglite/blob/master/tests/testthat/test-rect.R

test_that("rects equivalent regardless of direction", {
  ugd()
  plot.new()
  rect(0.2, 0.2, 0.8, 0.8)
  x1 <- xml2::read_xml(ugd_render())
  plot.new()
  rect(0.8, 0.8, 0.2, 0.2)
  x2 <- xml2::read_xml(ugd_render())
  dev.off()

  rect1 <- xml2::xml_attrs(xml2::xml_find_all(x1, "./d1:g/d1:rect")[[1]])
  rect2 <- xml2::xml_attrs(xml2::xml_find_all(x2, "./d1:g/d1:rect")[[1]])

  expect_equal(rect1, rect2)
})

test_that("fill and stroke colors", {

  x <- xmlSVG({
    plot.new()
    rect(0.2, 0.2, 0.8, 0.8, col = "blue", border = "red")
  })

  rectangle <- xml2::xml_find_all(x, "./d1:g/d1:rect")[[1]]
  expect_equal(style_attr(rectangle, "fill"), rgb(0, 0, 1))
  expect_equal(style_attr(rectangle, "stroke"), rgb(1, 0, 0))
})