#include "renderer_strings.h"

namespace unigd
{
namespace renderers
{

void RendererStrings::render(const Page& t_page, double t_scale)
{
  page(t_page);
}

void RendererStrings::get_data(const uint8_t** t_buf, size_t* t_size) const
{
  *t_buf = reinterpret_cast<const uint8_t*>(os.begin());
  *t_size = os.size();
}

void RendererStrings::page(const Page& t_page)
{
  string_count = 0;
  for (auto it = t_page.dcs.begin(); it != t_page.dcs.end(); ++it)
  {
    (*it)->visit(this);
  }
}

void RendererStrings::visit(const Rect* t_rect) {}

void RendererStrings::visit(const Text* t_text)
{
  if (string_count++ > 0)
  {
    fmt::format_to(std::back_inserter(os), "\n");
  }
  fmt::format_to(std::back_inserter(os), "{}", t_text->str);
}

void RendererStrings::visit(const Circle* t_circle) {}

void RendererStrings::visit(const Line* t_line) {}

void RendererStrings::visit(const Polyline* t_polyline) {}

void RendererStrings::visit(const Polygon* t_polygon) {}

void RendererStrings::visit(const Path* t_path) {}

void RendererStrings::visit(const Raster* t_raster) {}

}  // namespace renderers
}  // namespace unigd
