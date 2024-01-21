#' unigd: Universal graphics device
#'
#' Universal graphics device
#'
#' @name unigd-package
#' @useDynLib unigd, .registration=TRUE
"_PACKAGE"

.onLoad <- function(libname, pkgname) {
  unigd_ipc_open_()
}

#' @importFrom grDevices dev.list dev.off
.onUnload <- function (libpath) {
  ugd_close(all = TRUE)
  unigd_ipc_close_()
  library.dynam.unload("unigd", libpath)
}
