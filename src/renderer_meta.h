#ifndef __UNIGD_RENDERER_META_H__
#define __UNIGD_RENDERER_META_H__

#include <fmt/format.h>

#include "renderers.h"

namespace unigd
{
namespace renderers
{
class RendererMeta : public render_target
{
 public:
  void render(const Page &t_page, double t_scale) override;
  void get_data(const uint8_t **t_buf, size_t *t_size) const override;

  // Renderer
  void page(const Page &t_page);

 private:
  fmt::memory_buffer os;
  double m_scale;
};

}  // namespace renderers
}  // namespace unigd

#endif /* __UNIGD_RENDERER_META_H__ */
