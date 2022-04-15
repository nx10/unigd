#ifndef RENDERER_STRINGS_H
#define RENDERER_STRINGS_H

#include <unigd_api/draw_data.h>
#include <fmt/format.h>

namespace unigd::dc
{
    class RendererStrings : public Renderer
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        [[nodiscard]]
        void get_data(const uint8_t **t_buf, size_t *t_size) const override;

        // Renderer
        void page(const Page &t_page) override;
        void dc(const DrawCall &t_dc) override;
        void rect(const Rect &t_rect) override;
        void text(const Text &t_text) override;
        void circle(const Circle &t_circle) override;
        void line(const Line &t_line) override;
        void polyline(const Polyline &t_polyline) override;
        void polygon(const Polygon &t_polygon) override;
        void path(const Path &t_path) override;
        void raster(const Raster &t_raster) override;
    
    private:
        fmt::memory_buffer os;
        size_t string_count;
    };
    
} // namespace unigd::dc
#endif // RENDERER_STRINGS_H