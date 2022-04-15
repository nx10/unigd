#ifndef RENDERER_META_H
#define RENDERER_META_H

#include <unigd_api/draw_data.h>
#include <fmt/format.h>

namespace unigd::dc
{
    class RendererMeta : public Renderer
    {
    public:
        void render(const Page &t_page, double t_scale) override;
        [[nodiscard]]
        void get_data(const uint8_t **t_buf, size_t *t_size) const override;

        // Renderer
        void page(const Page &t_page) override;
    
    private:
        fmt::memory_buffer os;
        double m_scale;
    };
    
} // namespace unigd::dc
#endif // RENDERER_META_H