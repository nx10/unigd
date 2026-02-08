#ifndef __UNIGD_COMPRESS_H__
#define __UNIGD_COMPRESS_H__

#include <cstdint>
#include <string>
#include <vector>

namespace unigd
{
namespace compr
{
std::vector<uint8_t> compress(const uint8_t* input, size_t input_size);

std::vector<unsigned char> compress_str(const std::string& s);

}  // namespace compr
}  // namespace unigd

#endif /* __UNIGD_COMPRESS_H__ */
