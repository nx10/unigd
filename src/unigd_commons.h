
#ifndef UNIGD_COMMONS_H
#define UNIGD_COMMONS_H

#include <limits>
#include <string>
#include <vector>

namespace unigd
{
    // safely increases numbers (wraps to 0)
    template <typename T>
    T incwrap(T t_value)
    {
        T v = t_value;
        if (v == std::numeric_limits<T>::max())
        {
            return static_cast<T>(0);
        }
        return v + 1;
    }

    struct HttpgdState {
        int upid;
        size_t hsize;
        bool active;
    };

    struct HttpgdQueryResults {
        HttpgdState state;
        std::vector<int32_t> ids;
    };

} // namespace unigd

#endif