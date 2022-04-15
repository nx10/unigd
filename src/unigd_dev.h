#ifndef UNIGD_DEV_H
#define UNIGD_DEV_H

#include <cpp11/list.hpp>

#include <mutex>
#include <memory>
#include <iostream>
#include <compat/optional.hpp>

#include "generic_dev.h"
#include "unigd_commons.h"
#include "unigd_data_store.h"
#include "unigd_api_async.h"

#include "unigd_api/client.h"
#include "unigd_api/device.h"

#include "plot_history.h"

namespace unigd
{
    struct device_params
    {
        int bg;
        double width;
        double height;
        double pointsize;
        cpp11::list aliases;
        bool reset_par;
    };

    class DeviceTarget
    {
    public:
        [[nodiscard]] int get_index() const;
        void set_index(int index);
        [[nodiscard]] int get_newest_index() const;
        void set_newest_index(int index);
        [[nodiscard]] bool is_void() const;
        void set_void();

    private:
        int m_index{-1};        // current draw target
        int m_newest_index{-1}; // open draw target
        bool m_void{true};
    };

    class HttpgdDev : public devGeneric,
                      public device_api
    {
    public:

        // Font handling
        cpp11::list system_aliases;
        cpp11::list user_aliases;

        HttpgdDev(const device_params &t_params);
        virtual ~HttpgdDev();

        bool attach_client(const std::shared_ptr<graphics_client> &t_client);

        // API functions

        virtual void api_prerender(int index, double width, double height) override;
        virtual bool api_remove(int index) override;
        virtual bool api_clear() override;
        virtual device_state api_state() override;
        device_api_query_result api_query_all() override;
        device_api_query_result api_query_index(int index) override;
        device_api_query_result api_query_range(int offset, int limit) override;
        bool api_render(int index, double width, double height, dc::Renderer *t_renderer, double t_scale) override;
        virtual int api_index(int32_t id) override;

    protected:
        // Device callbacks

        virtual void dev_activate(pDevDesc dd) override;
        virtual void dev_deactivate(pDevDesc dd) override;
        virtual void dev_close(pDevDesc dd) override;
        virtual void dev_clip(double x0, double x1, double y0, double y1, pDevDesc dd) override;
        virtual void dev_size(double *left, double *right, double *bottom, double *top, pDevDesc dd) override;
        virtual void dev_newPage(pGEcontext gc, pDevDesc dd) override;
        virtual void dev_line(double x1, double y1, double x2, double y2, pGEcontext gc, pDevDesc dd) override;
        virtual void dev_text(double x, double y, const char *str, double rot, double hadj, pGEcontext gc, pDevDesc dd) override;
        virtual double dev_strWidth(const char *str, pGEcontext gc, pDevDesc dd) override;
        virtual void dev_rect(double x0, double y0, double x1, double y1, pGEcontext gc, pDevDesc dd) override;
        virtual void dev_circle(double x, double y, double r, pGEcontext gc, pDevDesc dd) override;
        virtual void dev_polygon(int n, double *x, double *y, pGEcontext gc, pDevDesc dd) override;
        virtual void dev_polyline(int n, double *x, double *y, pGEcontext gc, pDevDesc dd) override;
        virtual void dev_path(double *x, double *y, int npoly, int *nper, Rboolean winding, pGEcontext gc, pDevDesc dd) override;
        virtual void dev_mode(int mode, pDevDesc dd) override;
        virtual void dev_metricInfo(int c, pGEcontext gc, double *ascent, double *descent, double *width, pDevDesc dd) override;
        virtual void dev_raster(unsigned int *raster, int w, int h, double x, double y, double width, double height, double rot, Rboolean interpolate, pGEcontext gc, pDevDesc dd) override;

    private:
        PlotHistory m_history;
        std::shared_ptr<HttpgdDataStore> m_data_store;
        std::shared_ptr<HttpgdApiAsync> m_api_async_watcher;
        
        std::shared_ptr<graphics_client> m_client;

        bool replaying{false}; // Is the device replaying
        DeviceTarget m_target;

        bool m_initialized{false};

        void put(std::shared_ptr<dc::DrawCall> dc);

        // set device size
        void resize_device_to_page(pDevDesc dd);
        
        // graphical parameters for reseting
        cpp11::list m_reset_par;
    };

} // namespace unigd

#endif