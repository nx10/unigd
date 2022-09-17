
#include "unigd_data_store.h"
#include <cmath>
#include <iostream>
#include "unigd_commons.h"

// Do not include any R headers here!

namespace unigd
{
    inline bool HttpgdDataStore::m_valid_index(ex::plot_relative_t t_index)
    {
        const auto psize = static_cast<ex::plot_relative_t>(m_pages.size());
        return (psize > 0 && (t_index >= -psize && t_index < psize));
    }
    inline std::size_t HttpgdDataStore::m_index_to_pos(ex::plot_relative_t t_index)
    {
        return (t_index < 0 ? (m_pages.size() + t_index) : t_index);
    }

    std::experimental::optional<ex::plot_relative_t> HttpgdDataStore::normalize_index(ex::plot_relative_t t_index)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        if (!m_valid_index(t_index))
        {
            return std::experimental::nullopt;
        }
        return m_index_to_pos(t_index);
    }

    ex::plot_index_t HttpgdDataStore::append(gvertex<double> t_size)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        m_pages.emplace_back(m_id_counter, t_size);

        m_id_counter = incwrap(m_id_counter);

        return m_pages.size() - 1;
    }
    void HttpgdDataStore::add_dc(ex::plot_relative_t t_index, std::shared_ptr<renderers::DrawCall> t_dc, bool t_silent)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        if (!m_valid_index(t_index))
        {
            return;
        }
        auto index = m_index_to_pos(t_index);
        m_pages[index].put(t_dc);
        if (!t_silent)
        {
            m_inc_upid();
        }
    }

    void HttpgdDataStore::add_dc(ex::plot_relative_t t_index, const std::vector<std::shared_ptr<renderers::DrawCall>> &t_dcs, bool t_silent)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        if (!m_valid_index(t_index))
        {
            return;
        }
        auto index = m_index_to_pos(t_index);

        m_pages[index].put(t_dcs);
        if (!t_silent)
        {
            m_inc_upid();
        }
    }
    void HttpgdDataStore::clear(ex::plot_relative_t t_index, bool t_silent)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        if (!m_valid_index(t_index))
        {
            return;
        }
        auto index = m_index_to_pos(t_index);
        m_pages[index].clear();
        if (!t_silent)
        {
            m_inc_upid();
        }
    }
    bool HttpgdDataStore::remove(ex::plot_relative_t t_index, bool t_silent)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);

        if (!m_valid_index(t_index))
        {
            return false;
        }
        auto index = m_index_to_pos(t_index);

        m_pages.erase(m_pages.begin() + index);
        if (!t_silent) // if it was the last page
        {
            m_inc_upid();
        }
        return true;
    }
    bool HttpgdDataStore::remove_all()
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);

        if (m_pages.empty())
        {
            return false;
        }
        for (auto p : m_pages)
        {
            p.clear();
        }
        m_pages.clear();
        m_inc_upid();
        return true;
    }
    void HttpgdDataStore::fill(ex::plot_relative_t t_index, color_t t_fill)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        if (!m_valid_index(t_index))
        {
            return;
        }
        auto index = m_index_to_pos(t_index);
        m_pages[index].fill = t_fill;
    }
    void HttpgdDataStore::resize(ex::plot_relative_t t_index, gvertex<double> t_size)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        if (!m_valid_index(t_index))
        {
            return;
        }
        auto index = m_index_to_pos(t_index);
        m_pages[index].size = t_size;
        m_pages[index].clear();
    }
    unigd::gvertex<double> HttpgdDataStore::size(ex::plot_relative_t t_index)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        if (!m_valid_index(t_index))
        {
            return {10, 10};
        }
        auto index = m_index_to_pos(t_index);
        return m_pages[index].size;
    }
    void HttpgdDataStore::clip(ex::plot_relative_t t_index, grect<double> t_rect)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        if (!m_valid_index(t_index))
        {
            return;
        }
        auto index = m_index_to_pos(t_index);
        m_pages[index].clip(t_rect);
    }

    bool HttpgdDataStore::render(ex::plot_relative_t t_index, renderers::render_target *t_renderer, double t_scale)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        if (!m_valid_index(t_index))
        {
            return false;
        }
        auto index = m_index_to_pos(t_index);
        t_renderer->render(m_pages[index], std::fabs(t_scale));
        return true;
    }

    bool HttpgdDataStore::render_if_size(ex::plot_relative_t t_index, renderers::render_target *t_renderer, double t_scale, gvertex<double> t_target_size)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        if (!m_valid_index(t_index))
        {
            return false;
        }
        auto index = m_index_to_pos(t_index);

        // get current state
        gvertex<double> old_size = m_pages[index].size;

        if (t_target_size.x < 0.1)
        {
            t_target_size.x = old_size.x;
        }
        if (t_target_size.y < 0.1)
        {
            t_target_size.y = old_size.y;
        }

        // Check if replay needed
        if (std::fabs(t_target_size.x - old_size.x) > 0.1 ||
            std::fabs(t_target_size.y - old_size.y) > 0.1)
        {
            return false;
        }

        t_renderer->render(m_pages[index], std::fabs(t_scale));
        return true;
    }

    std::experimental::optional<ex::plot_index_t> HttpgdDataStore::find_index(ex::plot_id_t t_id)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        for (std::size_t i = 0; i != m_pages.size(); i++)
        {
            if (m_pages[i].id == t_id)
            {
                return static_cast<ex::plot_index_t>(i);
            }
        }
        return std::experimental::nullopt;
    }

    void HttpgdDataStore::m_inc_upid()
    {
        m_upid = incwrap(m_upid);
    }
    unigd_device_state HttpgdDataStore::state()
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        return {
            m_upid,
            static_cast<ex::plot_index_t>(m_pages.size()),
            m_device_active};
    }

    void HttpgdDataStore::set_device_active(bool t_active)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        m_device_active = t_active;
    }

    ex::find_results HttpgdDataStore::query(ex::plot_relative_t t_offset, ex::plot_id_t t_limit)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);

        if (!m_valid_index(t_offset))
        {
            return {{m_upid,
                     static_cast<ex::plot_index_t>(m_pages.size()),
                     m_device_active},
                    {}};
        }
        auto index = m_index_to_pos(t_offset);
        if (t_limit <= 0)
        {
            t_limit = m_pages.size();
        }
        auto end = std::min(m_pages.size(), index + static_cast<std::size_t>(t_limit));

        std::vector<ex::plot_id_t> res(end - index);
        for (std::size_t i = index; i != end; i++)
        {
            res[i - index] = m_pages[i].id;
        }
        return {{m_upid,
                 static_cast<ex::plot_index_t>(m_pages.size()),
                 m_device_active},
                res};
    }

    void HttpgdDataStore::extra_css(std::experimental::optional<std::string> t_extra_css)
    {
        const std::lock_guard<std::mutex> lock(m_store_mutex);
        m_extra_css = t_extra_css;
    }

} // namespace unigd
