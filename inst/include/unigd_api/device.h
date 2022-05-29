#ifndef RENDERER_MANAGER_H
#define RENDERER_MANAGER_H

#include <string>
#include <memory>
#include <vector>

namespace unigd
{
    using plot_id_t = int32_t;
    using plot_index_t = int32_t;
    using renderer_id_t = std::string;

    struct renderer_info
    {
        std::string id;
        std::string mime;
        std::string fileext;
        std::string name;
        std::string type;
        std::string description;
        bool text;
    };

    class render_data
    {
    public:
        virtual void get_data(const uint8_t **t_buf, size_t *t_size) const;
    };

    struct device_state {
        int upid;
        size_t hsize;
        bool active;
    };

    struct device_api_query_result {
        device_state state;
        std::vector<plot_id_t> ids;
    };

    class device_api
    {
    public:
        virtual bool api_render(renderer_id_t t_renderer, plot_id_t t_plot, double t_width, double t_height, double t_scale) = 0;

        virtual bool api_remove(plot_id_t id) = 0;
        virtual bool api_clear() = 0;

        virtual device_state api_state() = 0;

        virtual device_api_query_result api_query_all() = 0;
        virtual device_api_query_result api_query_index(plot_index_t index) = 0;
        virtual device_api_query_result api_query_range(plot_index_t offset, plot_index_t limit) = 0;
    };

} // namespace unigd

#endif // RENDERER_MANAGER_H