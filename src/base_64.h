#ifndef UNIGD_BASE64_H
#define UNIGD_BASE64_H

#include <unigd_api/draw_data.h>

namespace unigd
{
    std::string raster_base64(const dc::Raster &t_raster);

} // namespace unigd

#endif // UNIGD_BASE64_H