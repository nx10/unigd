#include "draw_data.h"

#include <iterator>

namespace unigd
{
namespace renderers
{

Text::Text(color_t t_col, gvertex<double> t_pos, std::string &&t_str, double t_rot,
           double t_hadj, TextInfo &&t_text)
    : col(t_col), pos(t_pos), rot(t_rot), hadj(t_hadj), str(t_str), text(t_text)
{
}
Circle::Circle(LineInfo &&t_line, color_t t_fill, gvertex<double> t_pos, double t_radius)
    : line(t_line), fill(t_fill), pos(t_pos), radius(t_radius)
{
}
Line::Line(LineInfo &&t_line, gvertex<double> t_orig, gvertex<double> t_dest)
    : line(t_line), orig(t_orig), dest(t_dest)
{
}
Rect::Rect(LineInfo &&t_line, color_t t_fill, grect<double> t_rect)
    : line(t_line), fill(t_fill), rect(t_rect)
{
}
Polyline::Polyline(LineInfo &&t_line, std::vector<gvertex<double>> &&t_points)
    : line(t_line), points(t_points)
{
}
Polygon::Polygon(LineInfo &&t_line, color_t t_fill,
                 std::vector<gvertex<double>> &&t_points)
    : line(t_line), fill(t_fill), points(t_points)
{
}
Path::Path(LineInfo &&t_line, color_t t_fill, std::vector<gvertex<double>> &&t_points,
           std::vector<int> &&t_nper, bool t_winding)
    : line(t_line), fill(t_fill), points(t_points), nper(t_nper), winding(t_winding)
{
}
Raster::Raster(std::vector<unsigned int> &&t_raster, gvertex<int> t_wh,
               grect<double> t_rect, double t_rot, bool t_interpolate)
    : raster(t_raster), wh(t_wh), rect(t_rect), rot(t_rot), interpolate(t_interpolate)
{
}

void Text::visit(draw_call_visitor *t_visitor) const { t_visitor->visit(this); }

void Circle::visit(draw_call_visitor *t_visitor) const { t_visitor->visit(this); }

void Line::visit(draw_call_visitor *t_visitor) const { t_visitor->visit(this); }

void Rect::visit(draw_call_visitor *t_visitor) const { t_visitor->visit(this); }

void Polyline::visit(draw_call_visitor *t_visitor) const { t_visitor->visit(this); }

void Polygon::visit(draw_call_visitor *t_visitor) const { t_visitor->visit(this); }

void Path::visit(draw_call_visitor *t_visitor) const { t_visitor->visit(this); }

void Raster::visit(draw_call_visitor *t_visitor) const { t_visitor->visit(this); }

Page::Page(page_id_t t_id, gvertex<double> t_size) : id(t_id), size(t_size), dcs(), cps()
{
  clip({0, 0, size.x, size.y});
}
void Page::put(std::unique_ptr<DrawCall> &&t_dc)
{
  t_dc->clip_id = cps.back().id;
  dcs.emplace_back(std::move(t_dc));
}
void Page::put(std::vector<std::unique_ptr<DrawCall>> &&t_dcs)
{
  for (auto &cp : t_dcs)
  {
    cp->clip_id = cps.back().id;
  }
  dcs.insert(dcs.end(), std::make_move_iterator(t_dcs.begin()),
             std::make_move_iterator(t_dcs.end()));
}
void Page::clear()
{
  dcs.clear();
  cps.clear();
  clip({0, 0, size.x, size.y});
}
void Page::clip(grect<double> t_rect)
{
  const auto cps_count = cps.size();
  if (cps_count == 0 || !cps.back().equals(t_rect))
  {
    cps.emplace_back(Clip{(int)cps_count, t_rect});
  }
}

}  // namespace renderers

}  // namespace unigd
