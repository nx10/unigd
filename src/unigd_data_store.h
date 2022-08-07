#ifndef UNIGD_SERVER_STORE_H
#define UNIGD_SERVER_STORE_H

#include "geom.h"
#include "renderers.h"
#include "unigd_external.h"
#include <compat/optional.hpp>

#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <vector>
#include <stdint.h>

namespace unigd
{
    using page_id_t = UNIGD_PLOT_ID;
    using page_index_t = UNIGD_PLOT_INDEX;

    class HttpgdDataStore
    {
    public:
        std::experimental::optional<page_index_t> find_index(page_id_t t_id);

        std::string svg(page_index_t t_index);
        bool render(page_index_t t_index, renderers::render_target *t_renderer, double t_scale);
        bool render_if_size(page_index_t t_index, renderers::render_target *t_renderer, double t_scale, gvertex<double> t_target_size);

        page_index_t append(gvertex<double> t_size);
        void clear(page_index_t t_index, bool t_silent);
        bool remove(page_index_t t_index, bool t_silent);
        bool remove_all();
        void resize(page_index_t t_index, gvertex<double> t_size);
        gvertex<double> size(page_index_t t_index);

        void fill(page_index_t t_index, color_t t_fill);
        void add_dc(page_index_t t_index, std::shared_ptr<renderers::DrawCall> t_dc, bool t_silent);
        void add_dc(page_index_t t_index, const std::vector<std::shared_ptr<renderers::DrawCall>> &t_dcs, bool t_silent);
        void clip(page_index_t t_index, grect<double> t_rect);

        ex::device_state state();
        void set_device_active(bool t_active);

        ex::find_results query_all();
        ex::find_results query_index(page_index_t t_index);
        ex::find_results query_range(page_index_t t_offset, page_index_t t_limit);

        void extra_css(std::experimental::optional<std::string> t_extra_css);

    private:
        std::mutex m_store_mutex;

        page_id_t m_id_counter = 0;
        std::vector<renderers::Page> m_pages;
        int m_upid = 0;
        bool m_device_active = true;

        std::experimental::optional<std::string> m_extra_css;

        void m_inc_upid();

        inline bool m_valid_index(page_index_t t_index);
        inline size_t m_index_to_pos(page_index_t t_index);
        
    };

} // namespace unigd

#endif // UNIGD_SERVER_H