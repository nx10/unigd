#ifndef RENDERER_SVG_H
#define RENDERER_SVG_H

#include "draw_data.h"
#include <fmt/format.h>
#include <compat/optional.hpp>
#include <string>

namespace unigd::dc
{
    class RendererSVG : public Renderer, public draw_call_visitor
    {
    public:
        explicit RendererSVG(std::experimental::optional<std::string> t_extra_css);
        void render(const Page &t_page, double t_scale) override;
        void get_data(const uint8_t **t_buf, size_t *t_size) const override;

        // Renderer
        void page(const Page &t_page);
        void visit(const Rect *t_rect) override;
        void visit(const Text *t_text) override;
        void visit(const Circle *t_circle) override;
        void visit(const Line *t_line) override;
        void visit(const Polyline *t_polyline) override;
        void visit(const Polygon *t_polygon) override;
        void visit(const Path *t_path) override;
        void visit(const Raster *t_raster) override;
    
    private:
        fmt::memory_buffer os;
        std::experimental::optional<std::string> m_extra_css;
        double m_scale;
    };

    /**
     * Produces SVG that can directly be embedded in HTML documents 
     * without causing ID conflicts at the expense of larger file size.
     * - Does not use style tags or CDATA embedded CSS.
     * - Appends random UUID to document-wide (clipPath) IDs.
     */
    class RendererSVGPortable : public Renderer, public draw_call_visitor
    {
    public:
        RendererSVGPortable();
        void render(const Page &t_page, double t_scale) override;
        void get_data(const uint8_t **t_buf, size_t *t_size) const override;

        // Renderer
        void page(const Page &t_page);
        void visit(const Rect *t_rect) override;
        void visit(const Text *t_text) override;
        void visit(const Circle *t_circle) override;
        void visit(const Line *t_line) override;
        void visit(const Polyline *t_polyline) override;
        void visit(const Polygon *t_polygon) override;
        void visit(const Path *t_path) override;
        void visit(const Raster *t_raster) override;
    
    private:
        fmt::memory_buffer os;
        double m_scale;
        std::string m_unique_id;
    };

    class RendererSVGZ : public RendererSVG
    {
    public:
        explicit RendererSVGZ(std::experimental::optional<std::string> t_extra_css);
        void render(const Page &t_page, double t_scale) override;
        void get_data(const uint8_t **t_buf, size_t *t_size) const override;

    private:
        std::vector<unsigned char> m_compressed;
    };
    
    class RendererSVGZPortable : public RendererSVGPortable
    {
    public:
        RendererSVGZPortable();
        void render(const Page &t_page, double t_scale) override;
        void get_data(const uint8_t **t_buf, size_t *t_size) const override;

    private:
        std::vector<unsigned char> m_compressed;
    };
    
} // namespace unigd::dc
#endif // RENDERER_SVG_H