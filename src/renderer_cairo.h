#ifndef RENDERER_CAIRO_H
#define RENDERER_CAIRO_H

#define UNIGD_NO_CAIRO
#ifndef UNIGD_NO_CAIRO

#include "draw_data.h"

#include <cairo.h>
#include <fmt/format.h>
#include <vector>

namespace unigd::renderers
{
    class RendererCairo : public Renderer
    {
    public:
        void page(const Page &t_page) override;
        void rect(const Rect &t_rect) override;
        void text(const Text &t_text) override;
        void circle(const Circle &t_circle) override;
        void line(const Line &t_line) override;
        void polyline(const Polyline &t_polyline) override;
        void polygon(const Polygon &t_polygon) override;
        void path(const Path &t_path) override;
        void raster(const Raster &t_raster) override;

    protected:
        cairo_surface_t *surface = nullptr;
        cairo_t *cr = nullptr;
    };

    class RendererCairoPng : public BinaryRenderingTarget, public RendererCairo
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        [[nodiscard]] 
        std::vector<unsigned char> get_binary() const override;
        
    private:
        std::vector<unsigned char> m_render_data{};
    };
    
    class RendererCairoPdf : public BinaryRenderingTarget, public RendererCairo
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        [[nodiscard]] 
        std::vector<unsigned char> get_binary() const override;
        
    private:
        std::vector<unsigned char> m_render_data{};
    };
    
    class RendererCairoPs : public StringRenderingTarget, public RendererCairo
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        [[nodiscard]] 
        std::string get_string() const override;
        
    private:
        fmt::memory_buffer m_os;
    };
    
    class RendererCairoEps : public StringRenderingTarget, public RendererCairo
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        [[nodiscard]] 
        std::string get_string() const override;
        
    private:
        fmt::memory_buffer m_os;
    };

    class RendererCairoTiff : public BinaryRenderingTarget, public RendererCairo
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        [[nodiscard]] 
        std::vector<unsigned char> get_binary() const override;
        
    private:
        std::vector<unsigned char> m_render_data{};
    };

} // namespace unigd::renderers

#endif

#endif // RENDERER_CAIRO_H