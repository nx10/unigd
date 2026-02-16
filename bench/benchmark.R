# unigd Benchmark Suite
#
# Compares unigd rendering performance against base R devices and alternatives.
# Requires: bench, unigd
# Optional: svglite, ragg, Cairo, ggplot2

run_benchmarks <- function(min_iterations = 5) {
  if (!requireNamespace("bench", quietly = TRUE)) {
    stop("Package 'bench' is required to run benchmarks.")
  }
  if (!requireNamespace("unigd", quietly = TRUE)) {
    stop("Package 'unigd' is required to run benchmarks.")
  }

  has_svglite  <- requireNamespace("svglite", quietly = TRUE)
  has_ragg     <- requireNamespace("ragg", quietly = TRUE)
  has_cairo    <- requireNamespace("Cairo", quietly = TRUE)
  has_ggplot2  <- requireNamespace("ggplot2", quietly = TRUE)

  # --- Test data (deterministic) ---
  set.seed(42)
  scatter_small_x <- rnorm(100)
  scatter_small_y <- rnorm(100)
  scatter_large_x <- rnorm(10000)
  scatter_large_y <- rnorm(10000)
  lines_mat <- matrix(cumsum(rnorm(5000)), ncol = 5)
  poly_data <- data.frame(
    x = runif(50, 0, 8),
    y = runif(50, 0, 8),
    w = runif(50, 0.5, 2),
    h = runif(50, 0.5, 2),
    col = sample(grDevices::hcl.colors(10, alpha = 0.4), 50, replace = TRUE)
  )
  text_x <- runif(200, 0, 10)
  text_y <- runif(200, 0, 10)
  text_labels <- paste0("L", seq_len(200))
  text_cex <- runif(200, 0.5, 2)

  if (has_ggplot2) {
    gg_df <- data.frame(
      x = rep(rnorm(200), 4),
      y = rep(rnorm(200), 4) + rep(c(0, 1, 2, 3), each = 200),
      facet = rep(paste("Panel", 1:4), each = 200)
    )
  }

  # --- Test plot closures ---
  plots <- list(
    scatter_small = function() {
      plot(scatter_small_x, scatter_small_y,
           main = "Small Scatter", xlab = "x", ylab = "y")
    },
    scatter_large = function() {
      plot(scatter_large_x, scatter_large_y,
           main = "Large Scatter", xlab = "x", ylab = "y", pch = ".")
    },
    lines = function() {
      matplot(lines_mat, type = "l", lty = 1, lwd = 1.5,
              main = "Time Series", xlab = "t", ylab = "value")
      legend("topleft", legend = paste("S", 1:5),
             col = 1:5, lty = 1, cex = 0.8)
    },
    polygons = function() {
      plot.new()
      plot.window(xlim = c(0, 10), ylim = c(0, 10))
      for (i in seq_len(nrow(poly_data))) {
        rect(poly_data$x[i], poly_data$y[i],
             poly_data$x[i] + poly_data$w[i],
             poly_data$y[i] + poly_data$h[i],
             col = poly_data$col[i], border = NA)
      }
      title("Overlapping Polygons")
    },
    text_heavy = function() {
      plot.new()
      plot.window(xlim = c(0, 10), ylim = c(0, 10))
      text(text_x, text_y, labels = text_labels, cex = text_cex)
      title("Text Heavy")
    }
  )
  if (has_ggplot2) {
    plots$ggplot_complex <- function() {
      p <- ggplot2::ggplot(gg_df, ggplot2::aes(x, y)) +
        ggplot2::geom_point(alpha = 0.5) +
        ggplot2::geom_smooth(method = "loess", formula = y ~ x) +
        ggplot2::facet_wrap(~facet) +
        ggplot2::theme_minimal()
      print(p)
    }
  }

  # --- Device wrappers ---
  # Each returns a function(plot_fn) that opens device, plots, closes,
  # and returns the output file size in bytes.

  # Traditional device: device opens a file, plot, dev.off
  wrap_traditional <- function(device_fn, ext, ...) {
    args <- list(...)
    function(plot_fn) {
      f <- tempfile(fileext = paste0(".", ext))
      on.exit(unlink(f), add = TRUE)
      do.call(device_fn, c(list(filename = f), args))
      plot_fn()
      dev.off()
      file.size(f)
    }
  }

  # Some base devices use 'file' instead of 'filename'
  wrap_traditional_file <- function(device_fn, ext, ...) {
    args <- list(...)
    function(plot_fn) {
      f <- tempfile(fileext = paste0(".", ext))
      on.exit(unlink(f), add = TRUE)
      do.call(device_fn, c(list(file = f), args))
      plot_fn()
      dev.off()
      file.size(f)
    }
  }

  # unigd device: ugd() -> plot -> ugd_save() -> dev.off()
  wrap_unigd <- function(renderer, ext, width = 720, height = 576) {
    function(plot_fn) {
      f <- tempfile(fileext = paste0(".", ext))
      on.exit(unlink(f), add = TRUE)
      unigd::ugd(width = width, height = height)
      plot_fn()
      unigd::ugd_save(file = f, as = renderer)
      dev.off()
      file.size(f)
    }
  }

  # Pixel dimensions for raster devices
  px_w <- 720
  px_h <- 576
  # Inch dimensions for vector devices (at 72 dpi)
  in_w <- 10
  in_h <- 8

  # --- Build device lists per group ---
  svg_devices <- list()
  svg_devices$`base::svg` <- wrap_traditional(
    grDevices::svg, "svg", width = in_w, height = in_h)
  if (has_svglite) {
    svg_devices$svglite <- wrap_traditional(
      svglite::svglite, "svg", width = in_w, height = in_h)
  }
  if (has_cairo) {
    svg_devices$`Cairo::CairoSVG` <- wrap_traditional_file(
      Cairo::CairoSVG, "svg", width = in_w, height = in_h)
  }
  svg_devices$`unigd svg` <- wrap_unigd("svg", "svg",
    width = in_w * 72, height = in_h * 72)

  png_devices <- list()
  png_devices$`base::png` <- wrap_traditional(
    grDevices::png, "png", width = px_w, height = px_h)
  if (has_ragg) {
    png_devices$`ragg::agg_png` <- wrap_traditional(
      ragg::agg_png, "png", width = px_w, height = px_h)
  }
  if (has_cairo) {
    png_devices$`Cairo::CairoPNG` <- wrap_traditional(
      Cairo::CairoPNG, "png", width = px_w, height = px_h)
  }
  png_devices$`unigd png` <- wrap_unigd("png", "png",
    width = px_w, height = px_h)

  pdf_devices <- list()
  pdf_devices$`base::pdf` <- wrap_traditional_file(
    grDevices::pdf, "pdf", width = in_w, height = in_h)
  if (has_cairo) {
    pdf_devices$`Cairo::CairoPDF` <- wrap_traditional_file(
      Cairo::CairoPDF, "pdf", width = in_w, height = in_h)
  }
  pdf_devices$`unigd pdf` <- wrap_unigd("pdf", "pdf",
    width = in_w * 72, height = in_h * 72)

  tiff_devices <- list()
  tiff_devices$`base::tiff` <- wrap_traditional(
    grDevices::tiff, "tiff", width = px_w, height = px_h)
  if (has_ragg) {
    tiff_devices$`ragg::agg_tiff` <- wrap_traditional(
      ragg::agg_tiff, "tiff", width = px_w, height = px_h)
  }
  tiff_devices$`unigd tiff` <- wrap_unigd("tiff", "tiff",
    width = px_w, height = px_h)

  groups <- list(
    SVG  = svg_devices,
    PNG  = png_devices,
    PDF  = pdf_devices,
    TIFF = tiff_devices
  )

  # --- Run benchmarks ---
  results <- list()

  for (group_name in names(groups)) {
    devices <- groups[[group_name]]
    if (length(devices) < 2) next

    message("=== ", group_name, " ===")

    for (plot_name in names(plots)) {
      plot_fn <- plots[[plot_name]]
      message("  Plot: ", plot_name)

      for (dev_name in names(devices)) {
        wrapper <- devices[[dev_name]]

        # Warmup
        tryCatch(wrapper(plot_fn), error = function(e) NULL)

        bm <- tryCatch(
          bench::mark(
            file_size = wrapper(plot_fn),
            min_iterations = min_iterations,
            check = FALSE,
            filter_gc = FALSE,
            memory = FALSE
          ),
          error = function(e) {
            message("    [", dev_name, "] error: ", conditionMessage(e))
            NULL
          }
        )

        if (!is.null(bm)) {
          # Collect file size from a single extra run
          fs <- tryCatch(wrapper(plot_fn), error = function(e) NA_real_)

          # Extract individual iteration times (seconds)
          iter_times <- as.numeric(bm$time[[1]])

          results <- c(results, list(data.frame(
            group     = group_name,
            device    = dev_name,
            plot      = plot_name,
            time      = iter_times,
            iteration = seq_along(iter_times),
            file_size = as.numeric(fs),
            stringsAsFactors = FALSE
          )))
        }
      }
    }
  }

  out <- do.call(rbind, results)
  rownames(out) <- NULL
  # Collect CPU info (best-effort, cross-platform)
  cpu <- tryCatch({
    sysname <- Sys.info()[["sysname"]]
    if (sysname == "Windows") {
      trimws(system("wmic cpu get name", intern = TRUE)[2])
    } else if (sysname == "Darwin") {
      trimws(system("sysctl -n machdep.cpu.brand_string", intern = TRUE))
    } else {
      lines <- readLines("/proc/cpuinfo", warn = FALSE)
      mn <- grep("^model name", lines, value = TRUE)[1]
      trimws(sub("^model name\\s*:\\s*", "", mn))
    }
  }, error = function(e) NA_character_)

  # Collect package versions
  pkg_names <- c("unigd", "bench", "svglite", "ragg", "Cairo", "ggplot2")
  pkg_versions <- vapply(pkg_names, function(p) {
    tryCatch(as.character(utils::packageVersion(p)),
             error = function(e) NA_character_)
  }, character(1))

  attr(out, "sysinfo") <- list(
    R.version = R.version.string,
    platform  = .Platform$OS.type,
    sysname   = Sys.info()[["sysname"]],
    machine   = Sys.info()[["machine"]],
    cpu       = cpu,
    packages  = pkg_versions,
    timestamp = Sys.time()
  )
  out
}

save_example_plots <- function(out_dir) {
  dir.create(out_dir, showWarnings = FALSE, recursive = TRUE)

  set.seed(42)
  scatter_small_x <- rnorm(100)
  scatter_small_y <- rnorm(100)
  scatter_large_x <- rnorm(10000)
  scatter_large_y <- rnorm(10000)
  lines_mat <- matrix(cumsum(rnorm(5000)), ncol = 5)
  poly_data <- data.frame(
    x = runif(50, 0, 8),
    y = runif(50, 0, 8),
    w = runif(50, 0.5, 2),
    h = runif(50, 0.5, 2),
    col = sample(grDevices::hcl.colors(10, alpha = 0.4), 50, replace = TRUE)
  )
  text_x <- runif(200, 0, 10)
  text_y <- runif(200, 0, 10)
  text_labels <- paste0("L", seq_len(200))
  text_cex <- runif(200, 0.5, 2)

  has_ggplot2 <- requireNamespace("ggplot2", quietly = TRUE)
  if (has_ggplot2) {
    gg_df <- data.frame(
      x = rep(rnorm(200), 4),
      y = rep(rnorm(200), 4) + rep(c(0, 1, 2, 3), each = 200),
      facet = rep(paste("Panel", 1:4), each = 200)
    )
  }

  plot_fns <- list(
    scatter_small = function() {
      plot(scatter_small_x, scatter_small_y,
           main = "Small Scatter", xlab = "x", ylab = "y")
    },
    scatter_large = function() {
      plot(scatter_large_x, scatter_large_y,
           main = "Large Scatter", xlab = "x", ylab = "y", pch = ".")
    },
    lines = function() {
      matplot(lines_mat, type = "l", lty = 1, lwd = 1.5,
              main = "Time Series", xlab = "t", ylab = "value")
      legend("topleft", legend = paste("S", 1:5),
             col = 1:5, lty = 1, cex = 0.8)
    },
    polygons = function() {
      plot.new()
      plot.window(xlim = c(0, 10), ylim = c(0, 10))
      for (i in seq_len(nrow(poly_data))) {
        rect(poly_data$x[i], poly_data$y[i],
             poly_data$x[i] + poly_data$w[i],
             poly_data$y[i] + poly_data$h[i],
             col = poly_data$col[i], border = NA)
      }
      title("Overlapping Polygons")
    },
    text_heavy = function() {
      plot.new()
      plot.window(xlim = c(0, 10), ylim = c(0, 10))
      text(text_x, text_y, labels = text_labels, cex = text_cex)
      title("Text Heavy")
    }
  )
  if (has_ggplot2) {
    plot_fns$ggplot_complex <- function() {
      p <- ggplot2::ggplot(gg_df, ggplot2::aes(x, y)) +
        ggplot2::geom_point(alpha = 0.5) +
        ggplot2::geom_smooth(method = "loess", formula = y ~ x) +
        ggplot2::facet_wrap(~facet) +
        ggplot2::theme_minimal()
      print(p)
    }
  }

  for (name in names(plot_fns)) {
    f <- file.path(out_dir, paste0("bench_plot_", name, ".png"))
    unigd::ugd(width = 360, height = 288)
    plot_fns[[name]]()
    unigd::ugd_save(file = f, as = "png")
    dev.off()
    message("Saved ", f)
  }

  invisible(names(plot_fns))
}

save_benchmark_charts <- function(results, out_dir) {
  if (!requireNamespace("ggplot2", quietly = TRUE)) {
    stop("Package 'ggplot2' is required to render benchmark charts.")
  }

  dir.create(out_dir, showWarnings = FALSE, recursive = TRUE)
  is_unigd <- function(name) grepl("^unigd ", name)
  pt_colors <- c("TRUE" = "#E64A19", "FALSE" = "steelblue")

  render_timing_chart <- function(df, group_title, file) {
    df$time_ms <- df$time * 1000
    df$is_unigd <- is_unigd(df$device)
    p <- ggplot2::ggplot(df, ggplot2::aes(
      x = device, y = time_ms, color = is_unigd
    )) +
      ggplot2::geom_jitter(width = 0.15, alpha = 0.35, size = 1.2) +
      ggplot2::stat_summary(fun = median, geom = "crossbar",
                            width = 0.5, linewidth = 0.5) +
      ggplot2::scale_color_manual(values = pt_colors, aesthetics = "colour",
                                  guide = "none") +
      ggplot2::facet_wrap(~plot, scales = "free") +
      ggplot2::scale_y_continuous(limits = c(0, NA),
                                  expand = ggplot2::expansion(mult = c(0, 0.05))) +
      ggplot2::coord_flip() +
      ggplot2::labs(
        title = paste(group_title, "- Iteration Times"),
        x = NULL, y = "Time (ms)"
      ) +
      ggplot2::theme_minimal(base_size = 11) +
      ggplot2::theme(strip.text = ggplot2::element_text(face = "bold"))
    ggplot2::ggsave(file, p, width = 8, height = 5, dpi = 150, bg = "white")
    message("Saved ", file)
  }

  for (grp in c("SVG", "PNG", "PDF", "TIFF")) {
    df <- results[results$group == grp, ]
    if (nrow(df) > 0) {
      render_timing_chart(df, grp,
        file.path(out_dir, paste0("bench_chart_", tolower(grp), ".png")))
    }
  }

  # File size chart
  agg <- aggregate(file_size ~ group + device, data = results,
                   FUN = function(x) x[1])
  agg <- agg[agg$file_size > 0, ]
  if (nrow(agg) > 0) {
    agg$size_kb <- agg$file_size / 1024
    agg <- agg[order(agg$group, agg$size_kb), ]
    agg$dev_ordered <- factor(
      paste(agg$group, agg$device, sep = "|||"),
      levels = paste(agg$group, agg$device, sep = "|||")
    )
    agg$is_unigd <- is_unigd(agg$device)
    p <- ggplot2::ggplot(agg, ggplot2::aes(
      x = dev_ordered, y = size_kb, fill = is_unigd
    )) +
      ggplot2::geom_col(show.legend = FALSE) +
      ggplot2::scale_fill_manual(values = pt_colors) +
      ggplot2::facet_wrap(~group, scales = "free") +
      ggplot2::coord_flip() +
      ggplot2::scale_x_discrete(labels = function(x) sub(".*\\|\\|\\|", "", x)) +
      ggplot2::labs(
        title = "File Size by Format",
        x = NULL, y = "Size (KB)"
      ) +
      ggplot2::theme_minimal(base_size = 11) +
      ggplot2::theme(strip.text = ggplot2::element_text(face = "bold"))
    ggplot2::ggsave(file.path(out_dir, "bench_chart_filesize.png"), p,
                    width = 8, height = 5, dpi = 150, bg = "white")
    message("Saved ", file.path(out_dir, "bench_chart_filesize.png"))
  }

  invisible(NULL)
}
