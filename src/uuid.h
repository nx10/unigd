#ifndef __UNIGD_UUID_H__
#define __UNIGD_UUID_H__

#include <string>

namespace unigd
{
// Can not use R's RNG for this for security reasons.
// (Seed could be predicted)
namespace uuid
{
std::string uuid();
}  // namespace uuid
}  // namespace unigd

#endif /* __UNIGD_UUID_H__ */
