
#include "unigd_dev.h"
#include "debug_print.h"

#include <cmath>
#include <cpp11/as.hpp>
#include <cpp11/doubles.hpp>
#include <cpp11/function.hpp>
#include <cpp11/list.hpp>
#include <cpp11/strings.hpp>
#include <memory>
#include <svglite_utils.h>
#include <string>

#include "r_thread.h"
#include "renderers.h"

namespace unigd
{
    static inline void r_graphics_par_set(cpp11::list t_par)
    {
        (cpp11::package("graphics")["par"])(t_par);
    }
    static inline cpp11::list r_graphics_par_get()
    {
        using namespace cpp11::literals;
        return cpp11::as_cpp<cpp11::list>(cpp11::package("graphics")["par"]("no.readonly"_nm = true));
    }

    int DeviceTarget::get_index() const
    {
        return m_index;
    }
    void DeviceTarget::set_index(int t_index)
    {
        m_void = false;
        m_index = t_index;
    }
    int DeviceTarget::get_newest_index() const
    {
        return m_newest_index;
    }
    void DeviceTarget::set_newest_index(int t_index)
    {
        m_newest_index = t_index;
    }
    bool DeviceTarget::is_void() const
    {
        return m_void;
    }
    void DeviceTarget::set_void()
    {
        m_void = true;
        m_index = -1;
    }

    unigd_device::unigd_device(const device_params &t_params)
        : generic_dev(t_params.width, t_params.height, t_params.pointsize, t_params.bg),
          system_aliases(cpp11::as_cpp<cpp11::list>(t_params.aliases["system"])),
          user_aliases(cpp11::as_cpp<cpp11::list>(t_params.aliases["user"])),
          m_history(),
          m_client(nullptr)
    {
        m_df_displaylist = true;

        m_data_store = std::make_shared<HttpgdDataStore>();

        m_reset_par = t_params.reset_par ? r_graphics_par_get() : cpp11::list();

        m_initialized = true;
    }

    bool unigd_device::attach_client(ex::graphics_client *t_client, void *t_client_data)
    {
        if (m_client)
        {
            return false;
        }
        m_client = t_client;
        m_client_data = t_client_data;
        m_client->start(m_client_data);
        return true;
    }

    bool unigd_device::get_client(ex::graphics_client **t_client, void **t_client_data)
    {
        if (!m_client)
        {
            return false;
        }
        *t_client = m_client;
        *t_client_data = m_client_data;
        return true;
    }

    bool unigd_device::remove_client()
    {
        if (!m_client)
        {
            return false;
        }
        m_client->close(m_client_data);
        m_client = nullptr;
        m_client_data = nullptr;
        return true;
    }

    // DEVICE CALLBACKS

    void unigd_device::dev_activate(pDevDesc dd)
    {
        if (!m_initialized)
            return;
        debug_println("ACTIVATE");
        m_data_store->set_device_active(true);
        if (m_client)
        {
            m_client->state_change(m_client_data);
        }
    }
    void unigd_device::dev_deactivate(pDevDesc dd)
    {
        if (!m_initialized)
            return;
        debug_println("DEACTIVATE");
        m_data_store->set_device_active(false);
        if (m_client)
        {
            m_client->state_change(m_client_data);
        }
    }

    void unigd_device::dev_mode(int mode, pDevDesc dd)
    {
        // debug_println("MODE %i", mode);
        if (m_target.is_void() || mode == 1)
            return;

        // flush buffer
        m_data_store->add_dc(m_target.get_index(), m_dc_buffer, replaying);
        m_dc_buffer.clear();

        if (m_client)
        {
            m_client->state_change(m_client_data);
        }
    }

    void unigd_device::dev_close(pDevDesc dd)
    {
        debug_println("CLOSE");
        m_initialized = false;

        // stop accepting draw calls
        m_target.set_void();
        m_target.set_newest_index(-1);

        // shutdown client
        remove_client();

        // cleanup r session data
        m_history.clear();
    }

    void unigd_device::dev_metricInfo(int c, pGEcontext gc, double *ascent, double *descent, double *width, pDevDesc dd)
    {
        if (c < 0)
        {
            c = -c;
        }

        FontSettings font = get_font_file(gc->fontfamily, gc->fontface, user_aliases);

        int error = glyph_metrics(c, font.file, font.index, gc->ps * gc->cex, 1e4, ascent, descent, width);
        if (error != 0)
        {
            *ascent = 0;
            *descent = 0;
            *width = 0;
        }
        double mod = 72. / 1e4;
        *ascent *= mod;
        *descent *= mod;
        *width *= mod;
    }
    double unigd_device::dev_strWidth(const char *str, pGEcontext gc, pDevDesc dd)
    {
        FontSettings font = get_font_file(gc->fontfamily, gc->fontface, user_aliases);

        double width = 0.0;

        int error = string_width(str, font.file, font.index, gc->ps * gc->cex, 1e4, 1, &width);

        if (error != 0)
        {
            width = 0.0;
        }

        return width * 72. / 1e4;
    }

    void unigd_device::dev_clip(double x0, double x1, double y0, double y1, pDevDesc dd)
    {
        if (m_target.is_void())
        {
            return;
        }
        m_data_store->clip(m_target.get_index(), normalize_rect(x0, y0, x1, y1));
    }
    void unigd_device::dev_size(double *left, double *right, double *bottom, double *top, pDevDesc dd)
    {
    }

    /**
     * "Figure margins too large" protection.
     * Including the graphics headers and reading the values directly
     * is about 40 times faster, but is probably not allowed by CRAN.
     */
    inline gvertex<double> find_minsize()
    {
        const auto mai = cpp11::as_cpp<cpp11::doubles>(r_graphics_par_get()["mai"]);
        const double minw = (mai[1] + mai[3]) * 72 + 1;
        const double minh = (mai[0] + mai[2]) * 72 + 1;
        return {minw, minh};
    }

    void unigd_device::resize_device_to_page(pDevDesc dd)
    {
        int index = (m_target.is_void()) ? m_target.get_newest_index() : m_target.get_index();

        auto size = m_data_store->size(index);
        auto minsize = find_minsize();

        dd->left = 0.0;
        dd->top = 0.0;
        dd->right = std::max(size.x, minsize.x);
        dd->bottom = std::max(size.y, minsize.y);
    }

    void unigd_device::dev_newPage(pGEcontext gc, pDevDesc dd)
    {
        const double width = dd->right;
        const double height = dd->bottom;
        const int fill = (R_ALPHA(gc->fill) == 0) ? dd->startfill : gc->fill;

        debug_print("[new_page] replaying=%i\n", replaying);
        if (!replaying)
        {
            if (m_target.get_newest_index() >= 0) // no previous pages
            {
                debug_print("    -> record open page in history\n");
                m_history.put_last(m_target.get_newest_index(), dd);
            }
            debug_print("    -> add new page to server\n");
            m_target.set_index(m_data_store->append({width, height}));
            m_target.set_newest_index(m_target.get_index());
        }
        else
        {
            debug_print("    -> rewrite target: %i\n", m_target.get_index());
            debug_print("    -> clear page\n");
            if (!m_target.is_void())
                m_data_store->clear(m_target.get_index(), true);
        }
        if (!m_target.is_void())
            m_data_store->fill(m_target.get_index(), fill);
    }

    inline renderers::LineInfo gc_lineinfo(pGEcontext gc)
    {
        return {
            gc->col,
            gc->lwd,
            gc->lty,
            static_cast<renderers::LineInfo::GC_lineend>(gc->lend),
            static_cast<renderers::LineInfo::GC_linejoin>(gc->ljoin),
            gc->lmitre};
    }
    inline color_t gc_fill(pGEcontext gc)
    {
        return gc->fill;
    }

    void unigd_device::dev_line(double x1, double y1, double x2, double y2, pGEcontext gc, pDevDesc dd)
    {
        put(std::make_shared<renderers::Line>(gc_lineinfo(gc), gvertex<double>{x1, y1}, gvertex<double>{x2, y2}));
    }
    void unigd_device::dev_text(double x, double y, const char *str, double rot, double hadj, pGEcontext gc, pDevDesc dd)
    {
        FontSettings font_info = get_font_file(gc->fontfamily, gc->fontface, user_aliases);

        int weight = get_font_weight(font_info.file, font_info.index);

        std::string feature = "";
        for (int i = 0; i < font_info.n_features; ++i)
        {
            feature += "'";
            feature += font_info.features[i].feature[0];
            feature += font_info.features[i].feature[1];
            feature += font_info.features[i].feature[2];
            feature += font_info.features[i].feature[3];
            feature += "' ";
            feature += font_info.features[i].setting;
            feature += (i == font_info.n_features - 1 ? ";" : ",");
        }

        put(std::make_shared<renderers::Text>(gc->col, gvertex<double>{x, y}, str, rot, hadj,
                                              renderers::TextInfo{
                                                  weight,
                                                  feature,
                                                  fontname(gc->fontfamily, gc->fontface, system_aliases, user_aliases, font_info),
                                                  gc->cex * gc->ps,
                                                  is_italic(gc->fontface),
                                                  dev_strWidth(str, gc, dd)}));
    }
    void unigd_device::dev_rect(double x0, double y0, double x1, double y1, pGEcontext gc, pDevDesc dd)
    {
        put(std::make_shared<renderers::Rect>(gc_lineinfo(gc), gc_fill(gc), normalize_rect(x0, y0, x1, y1)));
    }
    void unigd_device::dev_circle(double x, double y, double r, pGEcontext gc, pDevDesc dd)
    {
        put(std::make_shared<renderers::Circle>(gc_lineinfo(gc), gc_fill(gc), gvertex<double>{x, y}, r));
    }
    void unigd_device::dev_polygon(int n, double *x, double *y, pGEcontext gc, pDevDesc dd)
    {
        std::vector<gvertex<double>> points(n);
        for (int i = 0; i < n; ++i)
        {
            points[i] = {x[i], y[i]};
        }
        put(std::make_shared<renderers::Polygon>(gc_lineinfo(gc), gc_fill(gc), std::move(points)));
    }
    void unigd_device::dev_polyline(int n, double *x, double *y, pGEcontext gc, pDevDesc dd)
    {
        std::vector<gvertex<double>> points(n);
        for (int i = 0; i < n; ++i)
        {
            points[i] = {x[i], y[i]};
        }
        put(std::make_shared<renderers::Polyline>(gc_lineinfo(gc), std::move(points)));
    }
    void unigd_device::dev_path(double *x, double *y, int npoly, int *nper, Rboolean winding, pGEcontext gc, pDevDesc dd)
    {
        std::vector<int> vnper(nper, nper + npoly);
        int npoints = 0;
        for (const auto &val : vnper)
        {
            npoints += val;
        }
        std::vector<gvertex<double>> points(npoints);
        for (int i = 0; i < npoints; ++i)
        {
            points[i] = {x[i], y[i]};
        }

        put(std::make_shared<renderers::Path>(gc_lineinfo(gc), gc_fill(gc), std::move(points), std::move(vnper), winding));
    }
    void unigd_device::dev_raster(unsigned int *raster, int w, int h, double x, double y, double width, double height, double rot, Rboolean interpolate, pGEcontext gc, pDevDesc dd)
    {
        const double abs_height = std::fabs(height);
        const double abs_width = std::fabs(width);

        std::vector<unsigned int> vraster(raster, raster + (w * h));
        put(std::make_shared<renderers::Raster>(std::move(vraster), gvertex<int>{w, h}, grect<double>{x, y - abs_height, abs_width, abs_height}, rot, interpolate));
    }

    // OTHER

    void unigd_device::put(std::shared_ptr<renderers::DrawCall> dc)
    {
        if (m_target.is_void())
            return;

        // debug_println("DC put");
        m_dc_buffer.emplace_back(dc);
        // m_data_store->add_dc(m_target.get_index(), dc, replaying);
    }

    void unigd_device::plt_prerender(int index, double width, double height)
    {
        if (index == -1)
            index = m_target.get_newest_index();

        pDevDesc dd = get_active_pDevDesc();

        debug_print("[render_page] index=%i\n", index);

        replaying = true;
        m_data_store->resize(index, {width, height}); // this also clears
        if (index == m_target.get_newest_index())
        {
            m_target.set_index(index);
            debug_print("    -> open page. target_index=%i\n", m_target.get_index());
            resize_device_to_page(dd);
            PlotHistory::replay_current(dd); // replay active page
        }
        else
        {
            debug_print("    -> old page. target_newest_index=%i\n", m_target.get_newest_index());
            m_history.put_current(m_target.get_newest_index(), dd);

            m_target.set_index(index);
            resize_device_to_page(dd);
            m_history.play(m_target.get_index(), dd);
            m_target.set_void();
            resize_device_to_page(dd);
            m_history.play(m_target.get_newest_index(), dd); // recreate previous state
            m_target.set_index(m_target.get_newest_index()); // set target to open page for new draw calls
        }
        replaying = false;
    }

    bool unigd_device::plt_clear()
    {
        // clear store
        bool r = m_data_store->remove_all();

        // clear history
        m_history.clear();
        m_target.set_void();
        m_target.set_newest_index(-1);

        if (m_reset_par.size() != 0)
        {
            const auto par = cpp11::package("graphics")["par"];
            par(m_reset_par);
        }

        if (m_client)
        {
            m_client->state_change(m_client_data);
        }

        return r;
    }

    bool unigd_device::plt_remove(int index)
    {
        if (index == -1)
            index = m_target.get_newest_index();

        // remove from store
        bool r = m_data_store->remove(index, false);

        // remove from history

        pDevDesc dd = get_active_pDevDesc();

        debug_print("[hist_remove] index = %i\n", index);
        replaying = true;
        m_history.remove(index);
        if (index == m_target.get_newest_index() && index > 0)
        {
            debug_print("   -> last removed replay new last\n");
            m_target.set_index(m_target.get_newest_index() - 1);
            resize_device_to_page(dd);
            m_history.play(m_target.get_newest_index() - 1, dd); // recreate state of the element before last element
        }
        m_target.set_newest_index(m_target.get_newest_index() - 1);
        replaying = false;

        if (m_client)
        {
            m_client->state_change(m_client_data);
        }

        return r;
    }

    bool unigd_device::plt_render(int index, double width, double height, renderers::render_target *t_renderer, double t_scale)
    {
        debug_println("check cached size");
        if (m_data_store->render_if_size(index, t_renderer, t_scale, {width, height}))
        {
            return true;
        }
        else
        {
            debug_println("graphics engine rerender");
            plt_prerender(index, width, height);
        }
        debug_println("render");
        return m_data_store->render(index, t_renderer, t_scale);
    }

    int unigd_device::plt_index(int32_t id)
    {
        return m_data_store->find_index(id).value_or(-1);
    }

    ex::device_state unigd_device::plt_state()
    {
        return m_data_store->state();
    }

    ex::find_results unigd_device::plt_query_all()
    {
        return m_data_store->query_all();
    }
    ex::find_results unigd_device::plt_query_index(int index)
    {
        return m_data_store->query_index(index);
    }
    ex::find_results unigd_device::plt_query_range(int offset, int limit)
    {
        return m_data_store->query_range(offset, limit);
    }

    bool unigd_device::api_remove(int32_t id)
    {
        const auto plot_idx = plt_index(id);
        try
        {
            return async::r_thread([&]()
                                   { return plt_remove(plot_idx); })
                .get();
        }
        catch (...)
        {
        }
        return false;
    }
    bool unigd_device::api_clear()
    {
        try
        {
            return async::r_thread([&]()
                                   { return plt_clear(); })
                .get();
        }
        catch (...)
        {
        }
        return false;
    }

    std::unique_ptr<ex::render_data> unigd_device::api_render(ex::renderer_id_t t_renderer_id, int32_t t_plot_id, double t_width, double t_height, double t_scale)
    {
        const auto plot_idx = plt_index(t_plot_id);

        renderers::renderer_map_entry ren;
        auto fi_renderer = renderers::find(t_renderer_id, &ren);
        if (!fi_renderer)
        {
            return nullptr;
        }

        auto renderer = ren.generator();
        if (!m_data_store->render_if_size(plot_idx, renderer.get(), t_scale, {t_width, t_height}))
        {
            if (!async::r_thread([&]()
                                 { return plt_render(plot_idx, t_width, t_height, renderer.get(), t_scale); })
                     .get())
            {
                return nullptr;
            }
        }
        return std::move(renderer);
    }


} // namespace unigd
