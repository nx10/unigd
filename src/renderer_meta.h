#ifndef RENDERER_META_H
#define RENDERER_META_H

#include "draw_data.h"
#include <fmt/format.h>

namespace unigd::dc
{
    class RendererMeta : public StringRenderingTarget, public Renderer
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        [[nodiscard]]
        std::string get_string() const override;

        // Renderer
        void page(const Page &t_page) override;
    
    private:
        fmt::memory_buffer os;
        double m_scale;
    };
    
} // namespace unigd::dc
#endif // RENDERER_META_H