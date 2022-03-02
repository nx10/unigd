#pragma once

#include <string>

namespace unigd
{
    // Can not use R's RNG for this for security reasons.
    // (Seed could be predicted)
    namespace uuid
    {
        std::string uuid();
    } // namespace uuid
} // namespace unigd
