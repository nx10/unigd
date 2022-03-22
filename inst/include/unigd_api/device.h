#ifndef DEVICE_API_H
#define DEVICE_API_H

#include <unigd_api/draw_data.h>

namespace unigd
{

    struct device_state {
        int upid;
        size_t hsize;
        bool active;
    };

    struct device_api_query_result {
        device_state state;
        std::vector<int32_t> ids;
    };

    class device_api
    {
    public:
        virtual bool api_remove(int index) = 0;
        virtual bool api_clear() = 0;

        virtual void api_prerender(int index, double width, double height) = 0;
        virtual bool api_render(int index, double width, double height, dc::RenderingTarget *t_renderer, double t_scale) = 0;
        virtual int api_index(int32_t id) = 0;
        

        virtual device_state api_state() = 0;

        virtual device_api_query_result api_query_all() = 0;
        virtual device_api_query_result api_query_index(int index) = 0;
        virtual device_api_query_result api_query_range(int offset, int limit) = 0;
    };


} // namespace unigd


#endif // DEVICE_API_H