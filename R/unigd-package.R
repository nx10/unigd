#' unigd: Universal unified graphics device
#'
#' @docType package
#' @name unigd-package
#' @useDynLib unigd, .registration=TRUE
NULL

.onLoad <- function(libname, pkgname) {
  #unigd_ipc_open_()
  TRUE
}

#' @importFrom grDevices dev.list dev.off
.onUnload <- function (libpath) {
  #ugd_close(all = TRUE)
  #unigd_ipc_close_()
  library.dynam.unload("unigd", libpath)
}
