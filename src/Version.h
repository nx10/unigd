#pragma once

#define UNIGD_VERSION "1.3.0"

#ifndef UNIGD_NO_CAIRO
#include <cairo.h>
#define UNIGD_VERSION_CAIRO CAIRO_VERSION_STRING
#else
#define UNIGD_VERSION_CAIRO ""
#endif