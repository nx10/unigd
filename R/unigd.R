

#' Asynchronous HTTP server graphics device.
#'
#' This function initializes a unigd graphics device and
#' starts a local webserver, that allows for access via HTTP and WebSockets.
#' A link will be printed by which the web client can be accessed using
#' a browser.
#'
#' All font settings and descriptions are adopted from the excellent
#' 'svglite' package.
#'
#' @param width Graphics device width (pixels).
#' @param height Graphics device height (pixels).
#' @param bg Background color.
#' @param pointsize Graphics device point size.
#' @param system_fonts Named list of font names to be aliased with
#'   fonts installed on your system. If unspecified, the R default
#'   families `sans`, `serif`, `mono` and `symbol`
#'   are aliased to the family returned by
#'   [systemfonts::font_info()].
#' @param user_fonts Named list of fonts to be aliased with font files
#'   provided by the user rather than fonts properly installed on the
#'   system. The aliases can be fonts from the fontquiver package,
#'   strings containing a path to a font file, or a list containing
#'   `name` and `file` elements with `name` indicating
#'   the font alias in the SVG output and `file` the path to a
#'   font file.
#' @param reset_par If set to `TRUE`, global graphics parameters will be saved
#'   on device start and reset every time [ugd_clear()] is called (see
#'   [graphics::par()]).
#'
#' @return No return value, called to initialize graphics device.
#'
#' @importFrom systemfonts match_font
#' @export
#'
#' @examples
#' \dontrun{
#'
#' ugd() # Initialize graphics device and start server
#' ugd_browse() # Or copy the displayed link in the browser
#'
#' # Plot something
#' x <- seq(0, 3 * pi, by = 0.1)
#' plot(x, sin(x), type = "l")
#'
#' dev.off() # alternatively: ugd_close()
#' }
ugd <-
  function(width = getOption("unigd.width", 720),
           height = getOption("unigd.height", 576),
           bg = getOption("unigd.bg", "white"),
           pointsize = getOption("unigd.pointsize", 12),
           system_fonts = getOption("unigd.system_fonts", list()),
           user_fonts = getOption("unigd.user_fonts", list()),
           reset_par = getOption("unigd.reset_par", FALSE)) {

    aliases <- validate_aliases(system_fonts, user_fonts)

    invisible(unigd_ugd_(
      bg, width, height,
      pointsize, aliases,
      reset_par
    ))
  }

stop_if_not_unigd_device <- function(which) {
  if (names(which) != "unigd") {
    stop("Device is not of type unigd. (Start a device by calling: `ugd()`)")
  }
}

#' unigd device status.
#'
#' Access status information of a unigd graphics device.
#' This function will only work after starting a device with [ugd()].
#'
#' @param which Which device (ID).
#'
#' @return List of status variables with the following named items:
#'   `$hsize`: Plot history size (how many plots are accessible),
#'   `$upid`: Update ID (changes when the device has received new information),
#'   `$active`: Is the device the currently activated device.
#'
#' @importFrom grDevices dev.cur
#' @export
#'
#' @examples
#' \dontrun{
#'
#' ugd()
#' ugd_state()
#' plot(1, 1)
#' ugd_state()
#'
#' dev.off()
#' }
ugd_state <- function(which = dev.cur()) {
  stop_if_not_unigd_device(which)
  return(unigd_state_(which))
}

#' unigd device information.
#'
#' Access general information of a unigd graphics device.
#' This function will only work after starting a device with [ugd()].
#'
#' @param which Which device (ID).
#'
#' @return List of status variables with the following named items:
#'   `$id`: Server unique ID,
#'   `$version`: unigd and library versions.
#'
#' @importFrom grDevices dev.cur
#' @export
#'
#' @examples
#' \dontrun{
#'
#' ugd()
#' ugd_info()
#'
#' dev.off()
#' }
ugd_info <- function(which = dev.cur()) {
  stop_if_not_unigd_device(which)
  return(unigd_info_(which))
}

#' unigd device renderers.
#'
#' Get a list of available renderers.
#' This function will only work after starting a device with [ugd()].
#'
#' @return List of renderers with the following named items:
#'   `$id`: Renderer ID,
#'   `$mime`: File mime type,
#'   `$ext`: File extension,
#'   `$name`: Human readable name,
#'   `$type`: Renderer type (currently either `plot` or `other`),
#'   `$bin`: Is the file a binary blob or text.
#'
#' @importFrom grDevices dev.cur
#' @export
#'
#' @examples
#'
#' ugd_renderers()
#'
ugd_renderers <- function() {
  unigd_renderers_()
}

#' Query unigd plot IDs
#'
#' Query unigd graphics device static plot IDs.
#' Available plot IDs starting from `index` will be returned.
#' `limit` specifies the number of plots.
#' This function will only work after starting a device with [ugd()].
#'
#' @param index Plot index. If this is set to `0`, the last page will be
#'   selected.
#' @param limit Limit the number of returned IDs. If this is set to a
#'  value > 1 the returned type is a list if IDs. Set to `0` for all.
#' @param which Which device (ID).
#' @param state Include the current device state in the returned result
#'  (see also: [ugd_state()]).
#'
#' @return TODO
#'
#' @importFrom grDevices dev.cur
#' @export
#'
#' @examples
#' \dontrun{
#'
#' ugd()
#' plot.new()
#' text(.5, .5, "#1")
#' plot.new()
#' text(.5, .5, "#2")
#' plot.new()
#' text(.5, .5, "#3")
#' third <- ugd_id()
#' second <- ugd_id(2)
#' all <- ugd_id(1, limit = Inf)
#' ugd_remove(1)
#' ugd_plot(second)
#'
#' dev.off()
#' }
ugd_id <- function(index = 0, limit = 1, which = dev.cur(), state = FALSE) {
  stop_if_not_unigd_device(which)
  if (is.infinite(limit)) {
    limit <- 0
  }
  res <- unigd_id_(which, index - 1, limit)
  if (state) {
    return(res)
  }
  if (limit == 1 && length(res$plots) > 0) {
    return(res$plots[[1]])
  }
  return(res$plots)
}

page_id_to_index <- function(page, which) {
  if (inherits(page, "unigd_pid")) {
    print(page)
    print(which)
    page <- unigd_plot_find_(which, page$id) + 1
  }
  page
}

#' @export
print.unigd_pid <- function(x, ...) cat(x$id)

#' Render unigd plot and return it.
#'
#' See [ugd_save()] for saving rendered plots as files.
#' This function will only work after starting a device with [ugd()].
#'
#' @param page Plot page to render. If this is set to `0`, the last page will
#'   be selected. Can be set to a numeric plot index or plot ID
#'   (see [ugd_id()]).
#' @param width Width of the plot. If this is set to `-1`, the last width will
#'   be selected.
#' @param height Height of the plot. If this is set to `-1`, the last height
#'   will be selected.
#' @param zoom Zoom level. (For example: `2` corresponds to 200%, `0.5` would
#'   be 50%.)
#' @param as Renderer.
#' @param which Which device (ID).
#'
#' @return Rendered plot. Text renderers return strings, binary renderers
#'   return byte arrays.
#'
#' @importFrom grDevices dev.cur
#' @export
#'
#' @examples
#' \dontrun{
#'
#' ugd()
#' plot(1, 1)
#' ugd_render(width = 600, height = 400, as = "svg")
#'
#' dev.off()
#' }
ugd_render <- function(page = 0,
                     width = -1,
                     height = -1,
                     zoom = 1,
                     as = "svg",
                     which = dev.cur()) {
  stop_if_not_unigd_device(which)
  page <- page_id_to_index(page, which)
  unigd_render_(which, page - 1, width, height, zoom, as)
}

#' Render unigd plot to a file.
#'
#' See [ugd_render()] for acessing plot data directly in memory without
#' saving as a file.
#' This function will only work after starting a device with [ugd()].
#'
#' @param file Filepath to save plot.
#' @param page Plot page to render. If this is set to `0`, the last page will
#'   be selected. Can be set to a numeric plot index or plot ID
#'   (see [ugd_id()]).
#' @param width Width of the plot. If this is set to `-1`, the last width will
#'   be selected.
#' @param height Height of the plot. If this is set to `-1`, the last height
#'   will be selected.
#' @param zoom Zoom level. (For example: `2` corresponds to 200%, `0.5` would
#'   be 50%.)
#' @param as Renderer. When set to `"auto"` renderer is inferred from the file
#'   extension.
#' @param which Which device (ID).
#'
#' @return Rendered SVG string.
#'
#' @importFrom grDevices dev.cur
#' @importFrom tools file_ext
#' @export
#'
#' @examples
#' \dontrun{
#'
#' ugd()
#' plot(1, 1)
#' ugd_save(file = tempfile(), width = 600, height = 400, as = "png")
#'
#' dev.off()
#' }
ugd_save <- function(file,
                     page = 0,
                     width = -1,
                     height = -1,
                     zoom = 1,
                     as = "auto",
                     which = dev.cur()) {
  stop_if_not_unigd_device(which)
  page <- page_id_to_index(page, which)
  if (as == "auto") {
    as <- tolower(tools::file_ext(file))
    if (!(as %in% ugd_renderers()$id)) {
      stop("Renderer could not automatically be inferred from file extension.",
        " (Set the renderer explicitly with e.g. `ugd_save(..., as = \"svg\")`)")
    }
  }
  ret <- unigd_render_(which, page - 1, width, height, zoom, as)
  if (is.character(ret)) {
    writeLines(text = ret, con = file, useBytes = TRUE)
  } else {
    writeBin(object = ret, con = file)
  }
}

#' Remove a unigd plot page.
#'
#' This function will only work after starting a device with [ugd()].
#'
#' @param page Plot page to remove. If this is set to `0`, the last page will
#'   be selected. Can be set to a numeric plot index or plot ID
#'   (see [ugd_id()]).
#' @param which Which device (ID).
#'
#' @return Whether the page existed (and thereby was successfully removed).
#'
#' @importFrom grDevices dev.cur
#' @export
#'
#' @examples
#' \dontrun{
#'
#' ugd()
#' plot(1, 1) # page 1
#' hist(rnorm(100)) # page 2
#' ugd_remove(page = 1) # remove page 1
#'
#' dev.off()
#' }
ugd_remove <- function(page = 0, which = dev.cur()) {
  stop_if_not_unigd_device(which)
  if (inherits(page, "unigd_pid")) {
    return(unigd_remove_id_(which, page$id))
  }
  return(unigd_remove_(which, page - 1))
}

#' Clear all unigd plot pages.
#'
#' This function will only work after starting a device with [ugd()].
#'
#' @param which Which device (ID).
#'
#' @return Whether there were any pages to remove.
#'
#' @importFrom grDevices dev.cur
#' @export
#'
#' @examples
#' \dontrun{
#'
#' ugd()
#' plot(1, 1)
#' hist(rnorm(100))
#' ugd_clear()
#' hist(rnorm(100))
#'
#' dev.off()
#' }
ugd_clear <- function(which = dev.cur()) {
  stop_if_not_unigd_device(which)
  return(unigd_clear_(which))
}

#' Close unigd device.
#'
#' This achieves the same effect as [grDevices::dev.off()],
#' but will only close the device if it has the unigd type.
#'
#' @param which Which device (ID).
#' @param all Should all running unigd devices be closed.
#'
#' @return Number and name of the new active device (after the specified device
#'   has been shut down).
#'
#' @importFrom grDevices dev.cur dev.list dev.off
#' @export
#'
#' @examples
#' \dontrun{
#'
#' ugd()
#' ugd_browse() # open browser
#' hist(rnorm(100))
#' ugd_close() # Equvalent to dev.off()
#'
#' ugd()
#' ugd()
#' ugd()
#' ugd_close(all = TRUE)
#' }
ugd_close <- function(which = dev.cur(), all = FALSE) {
  if (all) {
    ds <- dev.list()
    invisible(lapply(ds[names(ds) == "unigd"], dev.off))
  } else {
    if (which != 1 && names(which(dev.list() == which)) == "unigd") {
      dev.off(which)
    }
  }
}

#' Inline plot rendering.
#'
#' Convenience function for quick inline plot rendering.
#' This is similar to [ugd_render()] but the plotting code
#' is specified inline and an unigd graphics device is managed
#' (created and closed) automatically. Starting a device with [ugd()] is
#' therefore not necessary.
#'
#' @param code Plotting code. See examples for more information.
#' @param page Plot page to render. If this is set to `0`, the last page will
#'   be selected. Can be set to a numeric plot index or plot ID
#'   (see [ugd_id()]).
#' @param width Width of the plot.
#' @param height Height of the plot.
#' @param zoom Zoom level. (For example: `2` corresponds to 200%, `0.5` would
#'   be 50%.)
#' @param as Renderer.
#' @param ... Additional parameters passed to `ugd(...)`
#'
#' @return Rendered plot. Text renderers return strings, binary renderers
#'   return byte arrays.
#' @export
#'
#' @examples
#' \dontrun{
#'
#' ugd_render_inline({
#'   hist(rnorm(100))
#' }, as = "svgz")
#'
#' s <- ugd_render_inline({
#'   plot.new()
#'   lines(c(0.5, 1, 0.5), c(0.5, 1, 1))
#' })
#' cat(s)
#' }
ugd_render_inline <- function(code,
                       page = 0,
                       width = getOption("unigd.width", 720),
                       height = getOption("unigd.height", 576),
                       zoom = 1,
                       as = "svg",
                       ...) {
  ugd(
    width = (width / zoom),
    height = (height / zoom),
    ...
  )
  tryCatch(code,
    finally = {
      tryCatch({
        s <- ugd_render(
          page = page,
          width = width,
          height = height,
          zoom = zoom,
          as = as
        )
      }, finally = {
        dev.off()
      })
    }
  )
  s
}


#' Inline plot rendering to a file.
#'
#' Convenience function for quick inline plot rendering.
#' This is similar to [ugd_save()] but the plotting code
#' is specified inline and an unigd graphics device is managed
#' (created and closed) automatically. Starting a device with [ugd()] is
#' therefore not necessary.
#'
#' @param code Plotting code. See examples for more information.
#' @param file Filepath to save plot.
#' @param page Plot page to render. If this is set to `0`, the last page will
#'   be selected. Can be set to a numeric plot index or plot ID
#'   (see [ugd_id()]).
#' @param width Width of the plot.
#' @param height Height of the plot.
#' @param zoom Zoom level. (For example: `2` corresponds to 200%, `0.5` would
#'   be 50%.)
#' @param as Renderer.
#' @param ... Additional parameters passed to `ugd(...)`
#'
#' @export
#'
#' @examples
#' \dontrun{
#'
#' ugd_save_inline({
#'   plot.new()
#'   lines(c(0.5, 1, 0.5), c(0.5, 1, 1))
#' }, file = "plot.svg")
#' }
ugd_save_inline <- function(code,
                       file,
                       page = 0,
                       width = getOption("unigd.width", 720),
                       height = getOption("unigd.height", 576),
                       zoom = 1,
                       as = "auto",
                       ...) {
  ugd(
    width = (width / zoom),
    height = (height / zoom),
    ...
  )
  tryCatch(code,
    finally = {
      tryCatch({
        ugd_save(
          file = file,
          page = page,
          width = width,
          height = height,
          zoom = zoom,
          as = as
        )
      }, finally = {
        dev.off()
      })
    }
  )
}
