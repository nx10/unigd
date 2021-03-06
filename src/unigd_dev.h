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

    class unigd_device : public devGeneric, public device_api
    {
    public:

        // Font handling
        cpp11::list system_aliases;
        cpp11::list user_aliases;

        unigd_device(const device_params &t_params);

        bool attach_client(const std::shared_ptr<graphics_client> &t_client);
        bool get_client(std::shared_ptr<graphics_client> *t_client);

        // Synchronous access

        void plt_prerender(int index, double width, double height);
        bool plt_remove(int index);
        bool plt_clear();
        device_state plt_state();
        device_api_query_result plt_query_all();
        device_api_query_result plt_query_index(int index);
        device_api_query_result plt_query_range(int offset, int limit);
        bool plt_render(int index, double width, double height, renderers::Renderer *t_renderer, double t_scale);
        int plt_index(int32_t id);

        // Asynchronous access

        std::unique_ptr<render_data> api_render(renderer_id_t t_renderer, plot_id_t t_plot, double t_width, double t_height, double t_scale) override;
        bool api_remove(plot_id_t id) override;
        bool api_clear() override;
        device_state api_state() override;
        device_api_query_result api_query_all() override;
        device_api_query_result api_query_index(plot_index_t index) override;
        device_api_query_result api_query_range(plot_index_t offset, plot_index_t limit) override;

    protected:
        // Device callbacks

        void dev_activate(pDevDesc dd) override;
        void dev_deactivate(pDevDesc dd) override;
        void dev_close(pDevDesc dd) override;
        void dev_clip(double x0, double x1, double y0, double y1, pDevDesc dd) override;
        void dev_size(double *left, double *right, double *bottom, double *top, pDevDesc dd) override;
        void dev_newPage(pGEcontext gc, pDevDesc dd) override;
        void dev_line(double x1, double y1, double x2, double y2, pGEcontext gc, pDevDesc dd) override;
        void dev_text(double x, double y, const char *str, double rot, double hadj, pGEcontext gc, pDevDesc dd) override;
        double dev_strWidth(const char *str, pGEcontext gc, pDevDesc dd) override;
        void dev_rect(double x0, double y0, double x1, double y1, pGEcontext gc, pDevDesc dd) override;
        void dev_circle(double x, double y, double r, pGEcontext gc, pDevDesc dd) override;
        void dev_polygon(int n, double *x, double *y, pGEcontext gc, pDevDesc dd) override;
        void dev_polyline(int n, double *x, double *y, pGEcontext gc, pDevDesc dd) override;
        void dev_path(double *x, double *y, int npoly, int *nper, Rboolean winding, pGEcontext gc, pDevDesc dd) override;
        void dev_mode(int mode, pDevDesc dd) override;
        void dev_metricInfo(int c, pGEcontext gc, double *ascent, double *descent, double *width, pDevDesc dd) override;
        void dev_raster(unsigned int *raster, int w, int h, double x, double y, double width, double height, double rot, Rboolean interpolate, pGEcontext gc, pDevDesc dd) override;

    private:
        PlotHistory m_history;
        std::shared_ptr<HttpgdDataStore> m_data_store;
        
        std::shared_ptr<graphics_client> m_client;

        bool replaying{false}; // Is the device replaying
        DeviceTarget m_target;

        bool m_initialized{false};

        void put(std::shared_ptr<renderers::DrawCall> dc);

        // set device size
        void resize_device_to_page(pDevDesc dd);
        
        // graphical parameters for reseting
        cpp11::list m_reset_par;

        std::vector<std::shared_ptr<unigd::renderers::DrawCall>> m_dc_buffer{};
    };

} // namespace unigd

#endif