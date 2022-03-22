#include "r_thread.h"
#include "unigd_api_async.h"

namespace unigd
{

    HttpgdApiAsync::HttpgdApiAsync(
        device_api *t_rdevice,
        std::shared_ptr<HttpgdDataStore> t_data_store)
        : m_rdevice(t_rdevice),
          m_rdevice_alive(true),
          m_data_store(t_data_store)
    {
    }

    bool HttpgdApiAsync::api_remove(int index)
    {
        const std::lock_guard<std::mutex> lock(m_rdevice_alive_mutex);
        if (!m_rdevice_alive)
            return false;

        try {
            return async::r_thread([&](){
                return this->m_rdevice->api_remove(index);
            }).get();
        } catch (...) {}
        return false;
    }
    bool HttpgdApiAsync::api_clear()
    {
        const std::lock_guard<std::mutex> lock(m_rdevice_alive_mutex);
        if (!m_rdevice_alive)
            return false;

        try {
            return async::r_thread([&](){
                return this->m_rdevice->api_clear();
            }).get();
        } catch (...) {}
        return false;
    }

    void HttpgdApiAsync::api_prerender(int index, double width, double height)
    {
        const std::lock_guard<std::mutex> lock(m_rdevice_alive_mutex);
        if (!m_rdevice_alive)
            return;

        async::r_thread([&](){
            this->m_rdevice->api_prerender(index, width, height);
        }).wait();        
    }
    
    bool HttpgdApiAsync::api_render(int index, double width, double height, dc::RenderingTarget *t_renderer, double t_scale) 
    {
        if (m_data_store->diff(index, {width, height}))
        {
            api_prerender(index, width, height); // use async render call
            // todo perform sync diff again and sync render svg
        }
        return m_data_store->render(index, t_renderer, t_scale);
    }

    int HttpgdApiAsync::api_index(int32_t id)
    {
        return m_data_store->find_index(id).value_or(-1);
    }

    device_state HttpgdApiAsync::api_state()
    {
        return m_data_store->state();
    }
    
    device_api_query_result HttpgdApiAsync::api_query_all()
    {
        return m_data_store->query_all();
    }
    device_api_query_result HttpgdApiAsync::api_query_index(int index)
    {
        return m_data_store->query_index(index);
    }
    device_api_query_result HttpgdApiAsync::api_query_range(int offset, int limit)
    {
        return m_data_store->query_range(offset, limit);
    }

    void HttpgdApiAsync::rdevice_destructing()
    {
        const std::lock_guard<std::mutex> lock(m_rdevice_alive_mutex);
        m_rdevice_alive = false;
    }

} // namespace unigd