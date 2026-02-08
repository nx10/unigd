#ifndef __UNIGD_RENDERER_TIKZ_H__
#define __UNIGD_RENDERER_TIKZ_H__

#include <fmt/format.h>

#include "renderers.h"

namespace unigd
{
namespace renderers
{
class RendererTikZ : public render_target, public draw_call_visitor
{
 public:
  void render(const Page& t_page, double t_scale) override;
  void get_data(const uint8_t** t_buf, size_t* t_size) const override;

  // Renderer
  void page(const Page& t_page);
  void visit(const Rect* t_rect) override;
  void visit(const Text* t_text) override;
  void visit(const Circle* t_circle) override;
  void visit(const Line* t_line) override;
  void visit(const Polyline* t_polyline) override;
  void visit(const Polygon* t_polygon) override;
  void visit(const Path* t_path) override;
  void visit(const Raster* t_raster) override;

 private:
  fmt::memory_buffer os;
  double m_scale;
};

}  // namespace renderers
}  // namespace unigd

#endif /* __UNIGD_RENDERER_TIKZ_H__ */
