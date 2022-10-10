
#include "renderers.h"

#include "renderer_cairo.h"
#include "renderer_json.h"
#include "renderer_meta.h"
#include "renderer_strings.h"
#include "renderer_svg.h"
#include "renderer_tikz.h"

namespace unigd
{
namespace renderers
{

static std::unordered_map<std::string, renderer_map_entry> renderer_map = {
    {"svg",
     {{"svg", "image/svg+xml", ".svg", "SVG", "plot", "Scalable Vector Graphics (SVG).",
       true},
      []()
      { return std::make_unique<renderers::RendererSVG>(std::experimental::nullopt); }}},
    {"svgp",
     {{"svgp", "image/svg+xml", ".svg", "Portable SVG", "plot",
       "Version of the SVG renderer that produces portable SVGs.", true},
      []() { return std::make_unique<renderers::RendererSVGPortable>(); }}},
    {"json",
     {{"json", "application/json", ".json", "JSON", "plot",
       "Plot data serialized to JSON format.", true},
      []() { return std::make_unique<renderers::RendererJSON>(); }}},
    {"tikz",
     {{"tikz", "text/plain", ".tex", "TikZ", "plot", "LaTeX TikZ code.", true},
      []() { return std::make_unique<renderers::RendererTikZ>(); }}},
    {"strings",
     {{"strings", "text/plain", ".txt", "Strings", "data",
       "List of strings contained in plot.", true},
      []() { return std::make_unique<renderers::RendererStrings>(); }}},
    {"meta",
     {{"meta", "application/json", ".json", "Meta", "data", "Plot meta information.",
       true},
      []() { return std::make_unique<renderers::RendererMeta>(); }}},
    {"svgz",
     {{"svgz", "image/svg+xml", ".svgz", "SVGZ", "plot",
       "Compressed Scalable Vector Graphics (SVGZ).", false},
      []()
      { return std::make_unique<renderers::RendererSVGZ>(std::experimental::nullopt); }}},
    {"svgzp",
     {{"svgzp", "image/svg+xml", ".svgz", "Portable SVGZ", "plot",
       "Version of the SVG renderer that produces portable SVGZs.", false},
      []() { return std::make_unique<renderers::RendererSVGZPortable>(); }}}

#ifndef UNIGD_NO_CAIRO
    ,
    {"ps",
     {
         {"ps", "application/postscript", ".ps", "PS", "plot", "PostScript (PS).", true},
         []() { return std::make_unique<renderers::RendererCairoPs>(); },
     }},
    {"eps",
     {{"eps", "application/postscript", ".eps", "EPS", "plot",
       "Encapsulated PostScript (EPS).", true},
      []() { return std::make_unique<renderers::RendererCairoEps>(); }}},

    {"png",
     {{"png", "image/png", ".png", "PNG", "plot", "Portable Network Graphics (PNG).",
       false},
      []() { return std::make_unique<renderers::RendererCairoPng>(); }}},

    {"png-base64",
     {{"png-base64", "text/plain", ".txt", "Base64 PNG", "plot",
       "Base64 encoded Portable Network Graphics (PNG).", true},
      []() { return std::make_unique<renderers::RendererCairoPngBase64>(); }}},

    {"pdf",
     {{"pdf", "application/pdf", ".pdf", "PDF", "plot",
       "Adobe Portable Document Format (PDF).", false},
      []() { return std::make_unique<renderers::RendererCairoPdf>(); }}},

#ifndef UNIGD_NO_TIFF
    {"tiff",
     {{"tiff", "image/tiff", ".tiff", "TIFF", "plot", "Tagged Image File Format (TIFF).",
       false},
      []() { return std::make_unique<renderers::RendererCairoTiff>(); }}}
#endif /* UNIGD_NO_TIFF */

#endif /* UNIGD_NO_CAIRO */
};

bool find(const std::string &id, renderer_map_entry *renderer)
{
  const auto it = renderer_map.find(id);
  if (it != renderer_map.end())
  {
    *renderer = it->second;
    return true;
  }
  return false;
}

bool find_generator(const std::string &id, renderer_gen *renderer)
{
  renderer_map_entry renderer_str;
  if (find(id, &renderer_str))
  {
    *renderer = renderer_str.generator;
    return true;
  }
  return false;
}

bool find_info(const std::string &id, unigd_renderer_info *renderer)
{
  renderer_map_entry renderer_str;
  if (find(id, &renderer_str))
  {
    *renderer = renderer_str.info;
    return true;
  }
  return false;
}

const std::unordered_map<std::string, renderer_map_entry> *renderers()
{
  return &renderer_map;
}

}  // namespace renderers
}  // namespace unigd