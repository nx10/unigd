#ifndef RENDERER_CAIRO_H
#define RENDERER_CAIRO_H

#ifndef UNIGD_NO_CAIRO

#include "draw_data.h"

#include "renderers.h"
#include <cairo.h>
#include <fmt/format.h>
#include <vector>

namespace unigd::renderers
{
    class RendererCairo : public draw_call_visitor
    {
    public:
        void visit(const Rect *t_rect) override;
        void visit(const Text *t_text) override;
        void visit(const Circle *t_circle) override;
        void visit(const Line *t_line) override;
        void visit(const Polyline *t_polyline) override;
        void visit(const Polygon *t_polygon) override;
        void visit(const Path *t_path) override;
        void visit(const Raster *t_raster) override;

        void render_page(const Page *t_page);

    protected:
        cairo_surface_t *surface = nullptr;
        cairo_t *cr = nullptr;
    };

    class RendererCairoPng : public render_target, public RendererCairo
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        void get_data(const uint8_t **t_buf, size_t *t_size) const override;
        
    private:
        std::vector<unsigned char> m_render_data{};
    };

    class RendererCairoPngBase64 : public render_target, public RendererCairo
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        void get_data(const uint8_t **t_buf, size_t *t_size) const override;
        
    private:
        std::string m_buf;
    };
    
    class RendererCairoPdf : public render_target, public RendererCairo
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        void get_data(const uint8_t **t_buf, size_t *t_size) const override;
        
    private:
        std::vector<unsigned char> m_render_data{};
    };
    
    class RendererCairoPs : public render_target, public RendererCairo
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        void get_data(const uint8_t **t_buf, size_t *t_size) const override;
        
    private:
        fmt::memory_buffer m_os;
    };
    
    class RendererCairoEps : public render_target, public RendererCairo
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        void get_data(const uint8_t **t_buf, size_t *t_size) const override;
        
    private:
        fmt::memory_buffer m_os;
    };

#ifndef UNIGD_NO_TIFF

    class RendererCairoTiff : public render_target, public RendererCairo
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        void get_data(const uint8_t **t_buf, size_t *t_size) const override;
        
    private:
        std::vector<unsigned char> m_render_data{};
    };

#endif

} // namespace unigd::renderers

#endif

#endif // RENDERER_CAIRO_H