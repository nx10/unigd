#' Plot a 16-panel test pattern for evaluating graphics devices.
#'
#' Renders a 4x4 grid of panels, each isolating a specific drawing primitive or
#' device feature. Covers all 8 DrawCall types (Rect, Circle, Line, Polygon,
#' Polyline, Path, Text, Raster) plus clipping, alpha blending, line styles,
#' text metrics, anti-aliasing hints, and point characters.
#'
#' @return Invisible \code{NULL}; called for the side effect of drawing.
#'
#' @export
#'
#' @examples
#' \dontrun{
#'
#' ugd_test_pattern()
#' }
ugd_test_pattern <- function() {
  old_par <- graphics::par(
    mfrow = c(4, 4),
    mar = c(0.5, 0.5, 1.5, 0.5),
    oma = c(0, 0, 2, 0)
  )
  on.exit(graphics::par(old_par))

  # --- Panel 1: Rect ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Rect", cex.main = 0.9)
  graphics::box()
  graphics::rect(0.05, 0.1, 0.55, 0.5, col = "lightblue", border = "blue", lwd = 2)
  graphics::rect(0.35, 0.35, 0.95, 0.75, border = "darkred", lty = 2, lwd = 2)
  hcl <- grDevices::hcl.colors(8)
  for (i in seq_along(hcl)) {
    x0 <- 0.05 + (i - 1) * 0.1125
    graphics::rect(x0, 0.8, x0 + 0.1, 0.95, col = hcl[i], border = NA)
  }

  # --- Panel 2: Circle ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Circle", cex.main = 0.9)
  graphics::box()
  radii <- c(0.18, 0.13, 0.08, 0.04)
  fills <- c("coral", "gold", "lightgreen", "cornflowerblue")
  borders <- c("darkred", "darkorange", "darkgreen", "darkblue")
  for (i in seq_along(radii)) {
    graphics::symbols(
      0.45, 0.45, circles = radii[i],
      inches = FALSE, add = TRUE,
      fg = borders[i], bg = fills[i], lwd = 2
    )
  }
  for (j in 1:5) {
    graphics::symbols(
      0.15 + (j - 1) * 0.18, 0.88, circles = 0.04,
      inches = FALSE, add = TRUE,
      fg = "gray40", bg = NA, lwd = 1
    )
  }

  # --- Panel 3: Polygon ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Polygon", cex.main = 0.9)
  graphics::box()
  # Triangle
  graphics::polygon(c(0.1, 0.35, 0.22), c(0.05, 0.05, 0.35),
    col = "lightblue", border = "blue", lwd = 2
  )
  # Concave arrow
  graphics::polygon(c(0.5, 0.9, 0.7, 0.9, 0.5), c(0.25, 0.05, 0.2, 0.35, 0.25),
    col = "aquamarine", border = "darkgreen", lwd = 2
  )
  # 5-pointed star
  star_angles <- seq(pi / 2, pi / 2 + 4 * pi, length.out = 6)[1:5]
  star_order <- c(1, 3, 5, 2, 4)
  sx <- 0.35 + 0.2 * cos(star_angles[star_order])
  sy <- 0.7 + 0.2 * sin(star_angles[star_order])
  graphics::polygon(sx, sy, col = "gold", border = "darkorange", lwd = 2)
  # Dashed no-fill polygon
  graphics::polygon(c(0.65, 0.95, 0.8), c(0.55, 0.55, 0.9),
    col = NA, border = "purple", lty = 2, lwd = 2
  )

  # --- Panel 4: Path ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Path", cex.main = 0.9)
  graphics::box()
  # Evenodd vs winding comparison: outer square with inner square hole
  outer_x <- c(0.05, 0.45, 0.45, 0.05)
  outer_y <- c(0.05, 0.05, 0.55, 0.55)
  inner_x <- c(0.15, 0.35, 0.35, 0.15)
  inner_y <- c(0.15, 0.15, 0.45, 0.45)
  # Evenodd: inner square becomes a hole
  graphics::polypath(
    c(outer_x, NA, inner_x), c(outer_y, NA, inner_y),
    col = "violet", border = "purple", rule = "evenodd"
  )
  graphics::text(0.25, 0.58, "evenodd", cex = 0.6)
  # Winding: inner square is filled (same winding direction)
  graphics::polypath(
    c(outer_x + 0.5, NA, inner_x + 0.5), c(outer_y, NA, inner_y),
    col = "violet", border = "purple", rule = "winding"
  )
  graphics::text(0.75, 0.58, "winding", cex = 0.6)
  # 3-ring nested path
  ring_n <- 60
  ring_t <- seq(0, 2 * pi, length.out = ring_n + 1)[-(ring_n + 1)]
  r1x <- 0.5 + 0.22 * cos(ring_t)
  r1y <- 0.8 + 0.12 * sin(ring_t)
  r2x <- 0.5 + 0.15 * cos(ring_t)
  r2y <- 0.80 + 0.08 * sin(ring_t)
  r3x <- 0.5 + 0.08 * cos(ring_t)
  r3y <- 0.80 + 0.04 * sin(ring_t)
  graphics::polypath(
    c(r1x, NA, r2x, NA, r3x), c(r1y, NA, r2y, NA, r3y),
    col = "steelblue", border = "navy", rule = "evenodd"
  )

  # --- Panel 5: Line Styles ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Line Styles", cex.main = 0.9)
  graphics::box()
  lty_names <- c("blank", "solid", "dashed", "dotted", "dotdash", "longdash", "twodash")
  for (i in 0:6) {
    ypos <- 0.92 - i * 0.12
    graphics::segments(0.3, ypos, 0.95, ypos, lty = i, lwd = 2)
    graphics::text(0.02, ypos, lty_names[i + 1], cex = 0.55, adj = c(0, 0.5))
  }

  # --- Panel 6: Caps & Joins ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Caps & Joins", cex.main = 0.9)
  graphics::box()
  lend_names <- c("round", "butt", "square")
  for (i in 0:2) {
    ypos <- 0.88 - i * 0.17
    graphics::segments(0.3, ypos, 0.95, ypos, lwd = 10, lend = i, col = "steelblue")
    graphics::segments(0.3, ypos, 0.95, ypos, lwd = 0.5, col = "red")
    graphics::text(0.02, ypos, lend_names[i + 1], cex = 0.55, adj = c(0, 0.5))
  }
  ljoin_names <- c("round", "mitre", "bevel")
  for (i in 0:2) {
    xmid <- 0.12 + i * 0.3
    graphics::lines(
      c(xmid - 0.08, xmid, xmid + 0.08),
      c(0.08, 0.3, 0.08),
      lwd = 8, ljoin = i, col = "steelblue"
    )
    graphics::lines(
      c(xmid - 0.08, xmid, xmid + 0.08),
      c(0.08, 0.3, 0.08),
      lwd = 0.5, col = "red"
    )
    graphics::text(xmid, 0.02, ljoin_names[i + 1], cex = 0.5)
  }

  # --- Panel 7: Polyline ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Polyline", cex.main = 0.9)
  graphics::box()
  # Sine wave
  t_sin <- seq(0, 2 * pi, length.out = 80)
  graphics::lines(
    0.05 + 0.9 * t_sin / (2 * pi),
    0.78 + 0.12 * sin(t_sin),
    col = "blue", lwd = 1.5
  )
  # Zigzag
  zx <- seq(0.05, 0.95, length.out = 20)
  zy <- ifelse(seq_along(zx) %% 2 == 0, 0.6, 0.45)
  graphics::lines(zx, zy, col = "darkred", lwd = 1.5)
  # Spiral
  t_sp <- seq(0, 4 * pi, length.out = 150)
  r_sp <- 0.02 + 0.15 * t_sp / (4 * pi)
  graphics::lines(
    0.5 + r_sp * cos(t_sp),
    0.2 + r_sp * sin(t_sp) * 0.8,
    col = "darkgreen", lwd = 1.5
  )

  # --- Panel 8: Raster ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Raster", cex.main = 0.9)
  graphics::box()
  # 4x4 checkerboard
  checker <- matrix(rep(c(0, 1, 1, 0), 4), nrow = 4, ncol = 4)
  checker_raster <- grDevices::as.raster(checker)
  graphics::rasterImage(checker_raster, 0.05, 0.55, 0.45, 0.95, interpolate = FALSE)
  graphics::text(0.25, 0.5, "no interp", cex = 0.5)
  graphics::rasterImage(checker_raster, 0.55, 0.55, 0.95, 0.95, interpolate = TRUE)
  graphics::text(0.75, 0.5, "interp", cex = 0.5)
  # HCL gradient
  grad_cols <- grDevices::hcl.colors(32)
  grad_mat <- matrix(grad_cols, nrow = 1)
  grad_raster <- grDevices::as.raster(grad_mat)
  graphics::rasterImage(grad_raster, 0.05, 0.25, 0.95, 0.45, interpolate = TRUE)
  # Rotated raster
  small_raster <- grDevices::as.raster(matrix(c("red", "green", "blue", "yellow"), nrow = 2))
  graphics::rasterImage(small_raster, 0.25, 0.02, 0.75, 0.2, angle = 30, interpolate = FALSE)

  # --- Panel 9: Fonts & Sizes ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Fonts & Sizes", cex.main = 0.9)
  graphics::box()
  font_labels <- c("plain", "bold", "italic", "bold-italic")
  for (i in 1:4) {
    graphics::text(0.05, 0.95 - (i - 1) * 0.13, font_labels[i],
      font = i, cex = 0.8, adj = c(0, 1)
    )
  }
  cex_vals <- c(0.5, 0.7, 0.9, 1.1, 1.3, 1.6)
  for (i in seq_along(cex_vals)) {
    graphics::text(0.05, 0.42 - (i - 1) * 0.07, paste0(cex_vals[i], "x"),
      cex = cex_vals[i], adj = c(0, 0.5)
    )
  }

  # --- Panel 10: Rotation & Alignment ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Rotation & Align", cex.main = 0.9)
  graphics::box()
  # Rotation fan
  rot_angles <- c(0, 45, 90, 135)
  graphics::points(0.3, 0.7, pch = 3, cex = 2, col = "red")
  for (angle in rot_angles) {
    graphics::text(0.3, 0.7, paste0(angle, "\u00B0"), srt = angle, cex = 0.7, adj = c(0, 0))
  }
  # Alignment grid
  graphics::points(0.75, 0.3, pch = 3, cex = 3, col = "red")
  graphics::text(0.75, 0.3, "BL", adj = c(0, 1), cex = 0.7)
  graphics::text(0.75, 0.3, "BR", adj = c(1, 1), cex = 0.7)
  graphics::text(0.75, 0.3, "TL", adj = c(0, 0), cex = 0.7)
  graphics::text(0.75, 0.3, "TR", adj = c(1, 0), cex = 0.7)

  # --- Panel 11: Math & Unicode ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Math & Unicode", cex.main = 0.9)
  graphics::box()
  graphics::text(0.5, 0.82,
    expression(bar(x) == sum(frac(x[i], n), i == 1, n)),
    cex = 0.9
  )
  graphics::text(0.5, 0.55,
    expression(integral(f(x) * dx, a, b)),
    cex = 0.9
  )
  graphics::text(0.5, 0.32, "\u00E9\u00E8 \u00F8\u00D8 \u00E5\u00C5 \u00E6\u00C6", cex = 1.0)
  graphics::text(0.5, 0.15, "\u03B1\u03B2\u03B3 \u03B4\u03B5\u03B6 \u03C0\u03A3\u03A9", cex = 1.0)

  # --- Panel 12: Text Metrics ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Text Metrics", cex.main = 0.9)
  graphics::box()
  metric_strings <- c("Aqg", "WIDE", "ij")
  metric_cex <- c(1.2, 0.9, 1.5)
  ypositions <- c(0.78, 0.5, 0.22)
  for (i in seq_along(metric_strings)) {
    s <- metric_strings[i]
    cx <- metric_cex[i]
    yp <- ypositions[i]
    sw <- graphics::strwidth(s, cex = cx)
    sh <- graphics::strheight(s, cex = cx)
    xp <- 0.5 - sw / 2
    graphics::rect(xp, yp, xp + sw, yp + sh,
      border = "red", lty = 2, lwd = 1
    )
    graphics::text(xp, yp, s, adj = c(0, 0), cex = cx)
  }

  # --- Panel 13: Clipping ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Clipping", cex.main = 0.9)
  graphics::box()
  # Draw reference clip boundary (before clipping)
  graphics::rect(0.2, 0.1, 0.8, 0.7, border = "gray50", lty = 3, lwd = 1)
  graphics::text(0.5, 0.75, "clip region", cex = 0.5, col = "gray50")
  # Set clip region
  graphics::clip(0.2, 0.8, 0.1, 0.7)
  # Draw shapes that extend beyond the clip
  graphics::symbols(0.5, 0.4, circles = 0.45,
    inches = FALSE, add = TRUE,
    fg = "blue", bg = "lightblue", lwd = 2
  )
  graphics::segments(0, 0.4, 1, 0.4, col = "red", lwd = 2)
  graphics::rect(0.1, 0.0, 0.9, 0.9, col = NA, border = "darkgreen", lwd = 2)
  # Reset clip to full region
  graphics::clip(
    graphics::grconvertX(0, "npc", "user"),
    graphics::grconvertX(1, "npc", "user"),
    graphics::grconvertY(0, "npc", "user"),
    graphics::grconvertY(1, "npc", "user")
  )
  # Re-draw the reference boundary on top
  graphics::rect(0.2, 0.1, 0.8, 0.7, border = "red", lty = 2, lwd = 1.5)
  graphics::text(0.5, 0.85, "shapes clipped", cex = 0.55)

  # --- Panel 14: Color & Alpha ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Color & Alpha", cex.main = 0.9)
  graphics::box()
  # Overlapping RGB circles at alpha 0.4
  alpha_cols <- c(
    grDevices::adjustcolor("red", alpha.f = 0.4),
    grDevices::adjustcolor("green", alpha.f = 0.4),
    grDevices::adjustcolor("blue", alpha.f = 0.4)
  )
  cx_pos <- c(0.35, 0.5, 0.65)
  cy_pos <- c(0.72, 0.68, 0.72)
  for (i in 1:3) {
    graphics::symbols(cx_pos[i], cy_pos[i], circles = 0.15,
      inches = FALSE, add = TRUE,
      fg = NA, bg = alpha_cols[i]
    )
  }
  # Alpha gradient bar
  n_grad <- 20
  alpha_seq <- seq(0.1, 1.0, length.out = n_grad)
  for (i in seq_len(n_grad)) {
    x0 <- 0.05 + (i - 1) * 0.9 / n_grad
    graphics::rect(x0, 0.1, x0 + 0.9 / n_grad, 0.35,
      col = grDevices::adjustcolor("steelblue", alpha.f = alpha_seq[i]),
      border = NA
    )
  }
  graphics::text(0.05, 0.06, "0.1", cex = 0.5, adj = c(0, 0.5))
  graphics::text(0.95, 0.06, "1.0", cex = 0.5, adj = c(1, 0.5))
  graphics::text(0.5, 0.42, "alpha", cex = 0.55)

  # --- Panel 15: Anti-aliasing ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "Anti-aliasing", cex.main = 0.9)
  graphics::box()
  # Thin lines at various angles from a common point
  aa_angles <- c(0, 15, 30, 45, 60, 75, 90)
  for (angle in aa_angles) {
    rad <- angle * pi / 180
    graphics::segments(0.15, 0.5, 0.15 + 0.35 * cos(rad), 0.5 + 0.35 * sin(rad),
      lwd = 1, col = "black"
    )
  }
  # Tiny circles
  for (r in c(0.005, 0.01, 0.015, 0.02, 0.03)) {
    graphics::symbols(0.7, 0.75 - r * 10, circles = r,
      inches = FALSE, add = TRUE,
      fg = "black", bg = "gray70"
    )
  }
  # 1px-wide rects
  for (i in 1:5) {
    yb <- 0.05 + (i - 1) * 0.06
    graphics::rect(0.6, yb, 0.95, yb + 0.005, col = "black", border = NA)
  }

  # --- Panel 16: pch 0-25 ---
  graphics::plot.new()
  graphics::plot.window(xlim = c(0, 1), ylim = c(0, 1))
  graphics::title(main = "pch 0-25", cex.main = 0.9)
  graphics::box()
  ncols <- 7
  for (k in 0:25) {
    col_idx <- k %% ncols
    row_idx <- k %/% ncols
    px <- 0.08 + col_idx * (0.88 / (ncols - 1))
    py <- 0.82 - row_idx * 0.25
    graphics::points(px, py, pch = k, bg = "gold", cex = 1.5)
    graphics::text(px, py - 0.08, as.character(k), cex = 0.45)
  }

  graphics::mtext("unigd test pattern", outer = TRUE, cex = 1.2, font = 2)
  invisible()
}
