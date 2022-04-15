#ifndef UNIGD_UNIGD_API_ASYNC_H
#define UNIGD_UNIGD_API_ASYNC_H

#include <string>
#include <memory>
#include <mutex>
#include <functional>
#include <vector>
#include <unigd_api/device.h>
#include "unigd_commons.h"
#include "unigd_data_store.h"
#include <compat/optional.hpp>

namespace unigd
{
    class PlotChangedEventListener
    {
    public:
        virtual ~PlotChangedEventListener() = default; 
        virtual void plot_changed(int upid) = 0;
    };

    class HttpgdApiAsync : public device_api
    {

    public:
        std::function<void ()> broadcast_notify_change;

        HttpgdApiAsync(device_api *t_rdevice, std::shared_ptr<HttpgdDataStore> t_data_store);
        virtual ~HttpgdApiAsync() = default; 

        // Calls that DO synchronize with R
        void api_prerender(int index, double width, double height) override;
        bool api_remove(int index) override;
        bool api_clear() override;


        // Calls that MAYBE synchronize with R
        bool api_render(int index, double width, double height, dc::Renderer *t_renderer, double t_scale) override;
        int api_index(int32_t id) override;
        
        // Calls that DONT synchronize with R
        device_state api_state() override;
        device_api_query_result api_query_all() override;
        device_api_query_result api_query_index(int index) override;
        device_api_query_result api_query_range(int offset, int limit) override;

        // this will block when a operation is running in another thread that needs the r device to be alive
        void rdevice_destructing();

    private:
        device_api *m_rdevice;
        bool m_rdevice_alive;
        std::mutex m_rdevice_alive_mutex;
        
        std::shared_ptr<HttpgdDataStore> m_data_store;
    };
} // namespace unigd

#endif