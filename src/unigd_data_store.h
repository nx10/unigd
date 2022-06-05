#ifndef UNIGD_SERVER_STORE_H
#define UNIGD_SERVER_STORE_H

#include <unigd_api/device.h>
#include "geom.h"
#include "draw_data.h"
#include <compat/optional.hpp>

#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace unigd
{
    using page_id_t = int32_t;
    using page_index_t = int;

    class HttpgdDataStore
    {
    public:
        std::experimental::optional<page_index_t> find_index(page_id_t t_id);

        std::string svg(page_index_t t_index);
        bool render(page_index_t t_index, dc::Renderer *t_renderer, double t_scale);
        bool render_if_size(page_index_t t_index, dc::Renderer *t_renderer, double t_scale, gvertex<double> t_target_size);

        page_index_t append(gvertex<double> t_size);
        void clear(page_index_t t_index, bool t_silent);
        bool remove(page_index_t t_index, bool t_silent);
        bool remove_all();
        void resize(page_index_t t_index, gvertex<double> t_size);
        gvertex<double> size(page_index_t t_index);

        void fill(page_index_t t_index, color_t t_fill);
        void add_dc(page_index_t t_index, std::shared_ptr<dc::DrawCall> t_dc, bool t_silent);
        void add_dc(page_index_t t_index, const std::vector<std::shared_ptr<dc::DrawCall>> &t_dcs, bool t_silent);
        void clip(page_index_t t_index, grect<double> t_rect);

        device_state state();
        void set_device_active(bool t_active);

        device_api_query_result query_all();
        device_api_query_result query_index(page_index_t t_index);
        device_api_query_result query_range(page_index_t t_offset, page_index_t t_limit);

        void extra_css(std::experimental::optional<std::string> t_extra_css);

    private:
        std::mutex m_store_mutex;

        page_id_t m_id_counter = 0;
        std::vector<dc::Page> m_pages;
        int m_upid = 0;
        bool m_device_active = true;

        std::experimental::optional<std::string> m_extra_css;

        void m_inc_upid();

        inline bool m_valid_index(page_index_t t_index);
        inline size_t m_index_to_pos(page_index_t t_index);
        
    };

} // namespace unigd

#endif // UNIGD_SERVER_H