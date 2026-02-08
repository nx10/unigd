#ifndef __UNIGD_BASE_64_H__
#define __UNIGD_BASE_64_H__

#include <cstdint>

#include "draw_data.h"

namespace unigd
{
std::string base64_encode(const std::uint8_t* buffer, size_t size);
std::string raster_base64(const renderers::Raster& t_raster);

}  // namespace unigd

#endif /* __UNIGD_BASE_64_H__ */
