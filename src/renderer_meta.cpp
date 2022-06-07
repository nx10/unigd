#include "renderer_meta.h"

namespace unigd::renderers
{
    void RendererMeta::render(const Page &t_page, double t_scale)
    {
        m_scale = t_scale;
        page(t_page);
    }

    void RendererMeta::get_data(const uint8_t **t_buf, size_t *t_size) const
    {
        *t_buf = reinterpret_cast<const uint8_t *>(os.begin());
        *t_size = os.size();
    }

    void RendererMeta::page(const Page &t_page)
    {
        fmt::format_to(std::back_inserter(os), "{{\n " R""("id": "{}", "w": {:.2f}, "h": {:.2f}, "scale": {:.2f}, clips: {}, draw_calls: {})"" "\n}}",
        t_page.id, t_page.size.x, t_page.size.y, m_scale, t_page.cps.size(), t_page.dcs.size());
    }
    
} // namespace unigd::renderers
