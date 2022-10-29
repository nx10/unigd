#ifndef __UNIGD_DRAW_DATA_H__
#define __UNIGD_DRAW_DATA_H__

#include <memory>
#include <string>
#include <vector>

#include "geom.h"

// Do not include any R headers here !

namespace unigd
{
namespace renderers
{

namespace color
{
constexpr color_t red_offset{0};
constexpr color_t green_offset{8};
constexpr color_t blue_offset{16};
constexpr color_t alpha_offset{24};

constexpr color_t byte_mask{0xFF};
constexpr color_t blue_mask{byte_mask << blue_offset};
constexpr color_t green_mask{byte_mask << green_offset};
constexpr color_t red_mask{byte_mask << red_offset};
constexpr color_t alpha_mask{byte_mask << alpha_offset};

constexpr color_t rgb(color_t r, color_t g, color_t b)
{
  return ((r << red_offset) | (g << green_offset) | (b << blue_offset) | alpha_mask);
}
constexpr color_t rgba(color_t r, color_t g, color_t b, color_t a)
{
  return ((r << red_offset) | (g << green_offset) | (b << blue_offset) |
          (a << alpha_offset));
}
constexpr color_t red(color_t x) { return (x >> red_offset) & byte_mask; }
constexpr color_t green(color_t x) { return (x >> green_offset) & byte_mask; }
constexpr color_t blue(color_t x) { return (x >> blue_offset) & byte_mask; }
constexpr color_t alpha(color_t x) { return (x >> alpha_offset) & byte_mask; }
constexpr bool opaque(color_t x) { return alpha(x) == byte_mask; }
constexpr bool transparent(color_t x) { return alpha(x) == 0; }
constexpr bool tranwhite(color_t x)
{
  return x == rgba(byte_mask, byte_mask, byte_mask, 0);
}

constexpr double byte_frac(color_t x) { return x / static_cast<double>(byte_mask); }
constexpr double red_frac(color_t x) { return byte_frac(red(x)); }
constexpr double green_frac(color_t x) { return byte_frac(green(x)); }
constexpr double blue_frac(color_t x) { return byte_frac(blue(x)); }
constexpr double alpha_frac(color_t x) { return byte_frac(alpha(x)); }
}  // namespace color

using clip_id_t = int;
using page_id_t = uint32_t;

// Data

struct LineInfo
{
  static const int TY_BLANK = 0;
  static const int TY_SOLID = -1;

  enum GC_lineend
  {
    GC_ROUND_CAP = 1,
    GC_BUTT_CAP = 2,
    GC_SQUARE_CAP = 3
  };

  enum GC_linejoin
  {
    GC_ROUND_JOIN = 1,
    GC_MITRE_JOIN = 2,
    GC_BEVEL_JOIN = 3
  };

  enum LTY
  {
    BLANK = -1,
    SOLID = 0,
    DASHED = 4 + (4 << 4),
    DOTTED = 1 + (3 << 4),
    DOTDASH = 1 + (3 << 4) + (4 << 8) + (3 << 12),
    LONGDASH = 7 + (3 << 4),
    TWODASH = 2 + (2 << 4) + (6 << 8) + (2 << 12)
  };

  color_t col;
  double lwd;
  int lty;
  GC_lineend lend;
  GC_linejoin ljoin;
  double lmitre;
};

struct TextInfo
{
  int weight;
  std::string features;
  std::string font_family;
  double fontsize;
  bool italic;
  double txtwidth_px;
};

// Draw calls

class Page;
class DrawCall;
class Rect;
class Text;
class Circle;
class Line;
class Polyline;
class Polygon;
class Path;
class Raster;

struct draw_call_visitor
{
  virtual void visit(const Rect *t_rect) = 0;
  virtual void visit(const Text *t_text) = 0;
  virtual void visit(const Circle *t_circle) = 0;
  virtual void visit(const Line *t_line) = 0;
  virtual void visit(const Polyline *t_polyline) = 0;
  virtual void visit(const Polygon *t_polygon) = 0;
  virtual void visit(const Path *t_path) = 0;
  virtual void visit(const Raster *t_raster) = 0;
};

class DrawCall
{
 public:
  virtual ~DrawCall() = default;
  virtual void visit(draw_call_visitor *t_visitor) const = 0;

  clip_id_t clip_id = 0;
};

class Text : public DrawCall
{
 public:
  Text(color_t t_col, gvertex<double> t_pos, std::string &&t_str, double t_rot,
       double t_hadj, TextInfo &&t_text);
  void visit(draw_call_visitor *t_visitor) const override;

  color_t col;
  gvertex<double> pos;
  double rot, hadj;
  std::string str;
  TextInfo text;
};

class Circle : public DrawCall
{
 public:
  Circle(LineInfo &&t_line, color_t t_fill, gvertex<double> t_pos, double t_radius);
  void visit(draw_call_visitor *t_visitor) const override;

  LineInfo line;
  color_t fill;
  gvertex<double> pos;
  double radius;
};

class Line : public DrawCall
{
 public:
  Line(LineInfo &&t_line, gvertex<double> t_orig, gvertex<double> t_dest);
  void visit(draw_call_visitor *t_visitor) const override;

  LineInfo line;
  gvertex<double> orig, dest;
};

class Rect : public DrawCall
{
 public:
  Rect(LineInfo &&t_line, color_t t_fill, grect<double> t_rect);
  void visit(draw_call_visitor *t_visitor) const override;

  LineInfo line;
  color_t fill;
  grect<double> rect;
};

class Polyline : public DrawCall
{
 public:
  Polyline(LineInfo &&t_line, std::vector<gvertex<double>> &&t_points);
  void visit(draw_call_visitor *t_visitor) const override;

  LineInfo line;
  std::vector<gvertex<double>> points;
};
class Polygon : public DrawCall
{
 public:
  Polygon(LineInfo &&t_line, color_t t_fill, std::vector<gvertex<double>> &&t_points);
  void visit(draw_call_visitor *t_visitor) const override;

  LineInfo line;
  color_t fill;
  std::vector<gvertex<double>> points;
};
class Path : public DrawCall
{
 public:
  Path(LineInfo &&t_line, color_t t_fill, std::vector<gvertex<double>> &&t_points,
       std::vector<int> &&t_nper, bool t_winding);
  void visit(draw_call_visitor *t_visitor) const override;

  LineInfo line;
  color_t fill;
  std::vector<gvertex<double>> points;
  std::vector<int> nper;
  bool winding;
};

class Raster : public DrawCall
{
 public:
  Raster(std::vector<unsigned int> &&t_raster, gvertex<int> t_wh, grect<double> t_rect,
         double t_rot, bool t_interpolate);
  void visit(draw_call_visitor *t_visitor) const override;

  std::vector<unsigned int> raster;
  gvertex<int> wh;
  grect<double> rect;
  double rot;
  bool interpolate;
};

class Clip
{
 public:
  inline bool equals(const grect<double> &t_rect) const
  {
    return rect_equals(t_rect, rect, 0.01);
  }

  clip_id_t id;
  grect<double> rect;
};

class Page
{
 public:
  Page(page_id_t t_id, gvertex<double> t_size);

  Page(const Page &) = delete;
  Page &operator=(Page &) = delete;
  Page &operator=(const Page &) = delete;

  Page(Page &&) = default;
  Page &operator=(Page &&) = default;

  void put(std::unique_ptr<DrawCall> &&t_dc);
  void put(std::vector<std::unique_ptr<DrawCall>> &&t_dcs);
  void clear();
  void clip(grect<double> t_rect);

  page_id_t id;
  gvertex<double> size;
  color_t fill;

  std::vector<std::unique_ptr<DrawCall>> dcs;
  std::vector<Clip> cps;
};

}  // namespace renderers

}  // namespace unigd

#endif /* __UNIGD_DRAW_DATA_H__ */
