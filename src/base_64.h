#ifndef UNIGD_BASE64_H
#define UNIGD_BASE64_H

#include "draw_data.h"

namespace unigd
{
    std::string base64_encode(const std::uint8_t *buffer, size_t size);
    std::string raster_base64(const renderers::Raster &t_raster);

} // namespace unigd

#endif // UNIGD_BASE64_H