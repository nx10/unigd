#ifndef UNIGD_DRAWDATA_H
#define UNIGD_DRAWDATA_H

#include <unigd_api/geom.h>

#include <memory>
#include <string>
#include <vector>

// Do not include any R headers here !

namespace unigd::dc
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
            return ((r << red_offset) | (g << green_offset) | (b << blue_offset) | (a << alpha_offset));
        }
        constexpr color_t red(color_t x) { return (x >> red_offset) & byte_mask; }
        constexpr color_t green(color_t x) { return (x >> green_offset) & byte_mask; }
        constexpr color_t blue(color_t x) { return (x >> blue_offset) & byte_mask; }
        constexpr color_t alpha(color_t x) { return (x >> alpha_offset) & byte_mask; }
        constexpr bool opaque(color_t x) { return alpha(x) == byte_mask; }
        constexpr bool transparent(color_t x) { return alpha(x) == 0; }
        constexpr bool tranwhite(color_t x) { return x == rgba(byte_mask, byte_mask, byte_mask, 0); }

        constexpr double byte_frac(color_t x) { return x / static_cast<double>(byte_mask); }
        constexpr double red_frac(color_t x) { return byte_frac(red(x)); }
        constexpr double green_frac(color_t x) { return byte_frac(green(x)); }
        constexpr double blue_frac(color_t x) { return byte_frac(blue(x)); }
        constexpr double alpha_frac(color_t x) { return byte_frac(alpha(x)); }
    }

    using clip_id_t = int;
    using page_id_t = int32_t;

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
            DASHED = 4 + (4<<4),
            DOTTED = 1 + (3<<4),
            DOTDASH = 1 + (3<<4) + (4<<8) + (3<<12),
            LONGDASH = 7 + (3<<4),
            TWODASH = 2 + (2<<4) + (6<<8) + (2<<12)
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

    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        inline virtual void page(const Page &t_page);
        inline virtual void dc(const DrawCall &t_dc);
        inline virtual void rect(const Rect &t_rect);
        inline virtual void text(const Text &t_text);
        inline virtual void circle(const Circle &t_circle);
        inline virtual void line(const Line &t_line);
        inline virtual void polyline(const Polyline &t_polyline);
        inline virtual void polygon(const Polygon &t_polygon);
        inline virtual void path(const Path &t_path);
        inline virtual void raster(const Raster &t_raster);

        inline virtual void render(const Page &t_page, double t_scale);
        inline virtual void get_data(const uint8_t **t_buf, size_t *t_size) const;
    };

    class Clip;

    struct DrawCall
    {
    public:
        virtual ~DrawCall() = default; 
        virtual inline void render(Renderer *t_renderer) const
        {
            t_renderer->dc(*this);
        }

        clip_id_t clip_id = 0;
    };

    class Text : public DrawCall
    {
    public:
        Text(color_t t_col, gvertex<double> t_pos, std::string &&t_str, double t_rot, double t_hadj, TextInfo &&t_text)
            : col(t_col), pos(t_pos), rot(t_rot), hadj(t_hadj), str(t_str), text(t_text)
        {
        }
        inline void render(Renderer *t_renderer) const override
        {
            t_renderer->text(*this);
        }

        color_t col;
        gvertex<double> pos;
        double rot, hadj;
        std::string str;
        TextInfo text;
    };

    class Circle : public DrawCall
    {
    public:
        Circle(LineInfo &&t_line, color_t t_fill, gvertex<double> t_pos, double t_radius)
            : line(t_line), fill(t_fill), pos(t_pos), radius(t_radius)
        {
        }
        inline void render(Renderer *t_renderer) const override
        {
            t_renderer->circle(*this);
        }

        LineInfo line;
        color_t fill;
        gvertex<double> pos;
        double radius;
    };

    class Line : public DrawCall
    {
    public:
        Line(LineInfo &&t_line, gvertex<double> t_orig, gvertex<double> t_dest)
            : line(t_line), orig(t_orig), dest(t_dest)
        {
        }
        inline void render(Renderer *t_renderer) const override
        {
            t_renderer->line(*this);
        }

        LineInfo line;
        gvertex<double> orig, dest;
    };

    class Rect : public DrawCall
    {
    public:
        Rect(LineInfo &&t_line, color_t t_fill, grect<double> t_rect)
            : line(t_line), fill(t_fill), rect(t_rect)
        {
        }
        inline void render(Renderer *t_renderer) const override
        {
            t_renderer->rect(*this);
        }

        LineInfo line;
        color_t fill;
        grect<double> rect;
    };

    class Polyline : public DrawCall
    {
    public:
        Polyline(LineInfo &&t_line, std::vector<gvertex<double>> &&t_points)
            : line(t_line), points(t_points)
        {
        }
        inline void render(Renderer *t_renderer) const override
        {
            t_renderer->polyline(*this);
        }

        LineInfo line;
        std::vector<gvertex<double>> points;
    };
    class Polygon : public DrawCall
    {
    public:
        Polygon(LineInfo &&t_line, color_t t_fill, std::vector<gvertex<double>> &&t_points)
            : line(t_line), fill(t_fill), points(t_points)
        {
        }
        inline void render(Renderer *t_renderer) const override
        {
            t_renderer->polygon(*this);
        }

        LineInfo line;
        color_t fill;
        std::vector<gvertex<double>> points;
    };
    class Path : public DrawCall
    {
    public:
        Path(LineInfo &&t_line, color_t t_fill, std::vector<gvertex<double>> &&t_points, std::vector<int> &&t_nper, bool t_winding)
         : line(t_line), fill(t_fill), points(t_points), nper(t_nper), winding(t_winding)
        {
        }
        inline void render(Renderer *t_renderer) const override
        {
            t_renderer->path(*this);
        }
           

        LineInfo line;
        color_t fill;
        std::vector<gvertex<double>> points;
        std::vector<int> nper;
        bool winding;
    };

    class Raster : public DrawCall
    {
    public:
        Raster(std::vector<unsigned int> &&t_raster, gvertex<int> t_wh,
               grect<double> t_rect,
               double t_rot,
               bool t_interpolate)
            : raster(t_raster), wh(t_wh), rect(t_rect), rot(t_rot), interpolate(t_interpolate)
        {
        }
        inline void render(Renderer *t_renderer) const override
        {
            t_renderer->raster(*this);
        }

        std::vector<unsigned int> raster;
        gvertex<int> wh;
        grect<double> rect;
        double rot;
        bool interpolate;
    };

    class Clip
    {
    public:
        Clip(clip_id_t t_id, grect<double> t_rect)
            : id(t_id), rect(t_rect)
        {
        }
        [[nodiscard]] inline bool equals(grect<double> t_rect) const
        {
            return rect_equals(t_rect, rect, 0.01);
        }

        clip_id_t id;
        grect<double> rect;
    };

    class Page
    {
    public:
        Page(page_id_t t_id, gvertex<double> t_size)
            : id(t_id), size(t_size)
        {
            clip({0, 0, size.x, size.y});
        }
        inline void put(std::shared_ptr<DrawCall> t_dc)
        {
            dcs.emplace_back(t_dc);
            t_dc->clip_id = cps.back().id;
        }
        inline void put(const std::vector<std::shared_ptr<DrawCall>> t_dcs)
        {
            for (auto cp : t_dcs) {
                cp->clip_id = cps.back().id;
            }
            dcs.insert(dcs.end(), t_dcs.begin(), t_dcs.end());
        }
        inline void clear()
        {
            dcs.clear();
            cps.clear();
            clip({0, 0, size.x, size.y});
        }
        inline void clip(grect<double> t_rect)
        {
            const auto cps_count = cps.size();
            if (cps_count == 0 || !cps.back().equals(t_rect))
            {
                cps.emplace_back(Clip(cps_count, t_rect));
            }
        }

        page_id_t id;
        gvertex<double> size;
        color_t fill;

        std::vector<std::shared_ptr<DrawCall>> dcs;
        std::vector<Clip> cps;
    };

    void Renderer::page(const Page &t_page)
    {
    }
    void Renderer::dc(const DrawCall &t_dc)
    {
    }
    void Renderer::rect(const Rect &t_rect)
    {
        dc(t_rect);
    }
    void Renderer::text(const Text &t_text)
    {
        dc(t_text);
    }
    void Renderer::circle(const Circle &t_circle)
    {
        dc(t_circle);
    }
    void Renderer::line(const Line &t_line)
    {
        dc(t_line);
    }
    void Renderer::polyline(const Polyline &t_polyline)
    {
        dc(t_polyline);
    }
    void Renderer::polygon(const Polygon &t_polygon)
    {
        dc(t_polygon);
    }
    void Renderer::path(const Path &t_path)
    {
        dc(t_path);
    }
    void Renderer::raster(const Raster &t_raster)
    {
        dc(t_raster);
    }

} // namespace unigd::dc

#endif /* UNIGD_DRAWDATA_H */