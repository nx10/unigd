
#include "renderers.h"

#include "renderer_svg.h"
#include "renderer_json.h"
#include "renderer_cairo.h"
#include "renderer_tikz.h"
#include "renderer_strings.h"
#include "renderer_meta.h"


namespace unigd
{
    namespace renderers 
    {

  
    static std::unordered_map<std::string, renderer_map_entry> renderer_map = 
    {
      {
        "svg",
        {
          {
            "svg",
            "image/svg+xml",
            ".svg",
            "SVG",
            "plot",
            "Scalable Vector Graphics (SVG).",
            true
          },
          []() { return std::make_unique<dc::RendererSVG>(std::experimental::nullopt); }
        }
      },
      {
        "svgp",
        {
          {
            "svgp",
            "image/svg+xml",
            ".svg",
            "Portable SVG",
            "plot",
            "Version of the SVG renderer that produces portable SVGs.",
            true
          },
          []() { return std::make_unique<dc::RendererSVGPortable>(); }
        }
      },
      {
        "json",
        {
          {
            "json",
            "application/json",
            ".json",
            "JSON",
            "plot",
            "Plot data serialized to JSON format.",
            true
          },
          []() { return std::make_unique<dc::RendererJSON>(); }
        }
      },
      {
        "tikz",
        {
          {
            "tikz",
            "text/plain",
            ".tex",
            "TikZ",
            "plot",
            "LaTeX TikZ code.",
            true
          },
          []() { return std::make_unique<dc::RendererTikZ>(); }
        }
      },
      {
        "strings",
        {
          {
            "strings",
            "text/plain",
            ".txt",
            "Strings",
            "data",
            "List of strings contained in plot.",
            true
          },
          []() { return std::make_unique<dc::RendererStrings>(); }
        }
      },
      {
        "meta",
        {
          {
            "meta",
            "application/json",
            ".json",
            "Meta",
            "data",
            "Plot meta information.",
            true
          },
          []() { return std::make_unique<dc::RendererMeta>(); }
        }
      },
      {
        "svgz",
        {
          {
            "svgz",
            "image/svg+xml",
            ".svgz",
            "SVGZ",
            "plot",
            "Compressed Scalable Vector Graphics (SVGZ).",
            false
          },
          []() { return std::make_unique<dc::RendererSVGZ>(std::experimental::nullopt); }
        }
      },
      {
        "svgzp",
        {
          {
            "svgzp",
            "image/svg+xml",
            ".svgz",
            "Portable SVGZ",
            "plot",
            "Version of the SVG renderer that produces portable SVGZs.",
            false
          },
          []() { return std::make_unique<dc::RendererSVGZPortable>(); }
        }
      }

#ifndef UNIGD_NO_CAIRO
      ,
      {"ps",{
        {"ps",
        "application/postscript",
        ".ps",
        "PS",
        "plot",
        "PostScript (PS)."},
        []() { return std::make_unique<dc::RendererCairoPs>(); },
      }},
      {"eps",{
        {"eps",
        "application/postscript",
        ".eps",
        "EPS",
        "plot",
        "Encapsulated PostScript (EPS)."},
        []() { return std::make_unique<dc::RendererCairoEps>(); }
      }},
      
      {"png",{
        {"png",
        "image/png",
        ".png",
        "PNG",
        "plot",
        "Portable Network Graphics (PNG)."},
        []() { return std::make_unique<dc::RendererCairoPng>(); }
      }},
      
      {"pdf",{
        {"pdf",
        "application/pdf",
        ".pdf",
        "PDF",
        "plot",
        "Adobe Portable Document Format (PDF)."},
        []() { return std::make_unique<dc::RendererCairoPdf>(); }
      }},
      
      {"tiff",{
        {"tiff",
        "image/tiff",
        ".tiff",
        "TIFF",
        "plot",
        "Tagged Image File Format (TIFF)."},
        []() { return std::make_unique<dc::RendererCairoTiff>(); }
      }}
#endif
    };

    bool find(const std::string &id, const renderer_map_entry **renderer)
    {
        const auto it = renderer_map.find(id);
        if (it != renderer_map.end())
        {
            *renderer = &it->second;
            return true;
        }
        return false;
    }

    bool find_generator(const std::string &id, const renderer_gen **renderer)
    {
        const renderer_map_entry *renderer_str = nullptr;
        if (find(id, &renderer_str))
        {
            *renderer = &renderer_str->generator;
            return true;
        }
        return false;
    }

    bool find_info(const std::string &id, const renderer_info **renderer)
    {
        const renderer_map_entry *renderer_str = nullptr;
        if (find(id, &renderer_str))
        {
            *renderer = &renderer_str->info;
            return true;
        }
        return false;
    }

    const std::unordered_map<std::string, renderer_map_entry> *renderers() {
      return &renderer_map;
    }

/*
    const RendererManager RendererManager::generate_default() 
    {
        RendererManager manager;

        manager.add({
          "svg",
          "image/svg+xml",
          ".svg",
          "SVG",
          "plot",
          []() { return std::make_unique<dc::RendererSVG>(std::experimental::nullopt); },
          "Scalable Vector Graphics (SVG)."
        });
        
        manager.add(BinaryRendererInfo{
          "svgz",
          "image/svg+xml",
          ".svgz",
          "SVGZ",
          "plot",
          []() { return std::make_unique<dc::RendererSVGZ>(std::experimental::nullopt); },
          "Compressed Scalable Vector Graphics (SVGZ)."
        });
        
        manager.add({
          "svgp",
          "image/svg+xml",
          ".svg",
          "Portable SVG",
          "plot",
          []() { return std::make_unique<dc::RendererSVGPortable>(); },
          "Version of the SVG renderer that produces portable SVGs."
        });
        
        manager.add(BinaryRendererInfo{
          "svgzp",
          "image/svg+xml",
          ".svgz",
          "Portable SVGZ",
          "plot",
          []() { return std::make_unique<dc::RendererSVGZPortable>(); },
          "Version of the SVG renderer that produces portable SVGZs."
        });
        
#ifndef UNIGD_NO_CAIRO
        
        manager.add({
          "png",
          "image/png",
          ".png",
          "PNG",
          "plot",
          []() { return std::make_unique<dc::RendererCairoPng>(); },
          "Portable Network Graphics (PNG)."
        });
        
        manager.add({
          "pdf",
          "application/pdf",
          ".pdf",
          "PDF",
          "plot",
          []() { return std::make_unique<dc::RendererCairoPdf>(); },
          "Adobe Portable Document Format (PDF)."
        });
        
        manager.add({
          "ps",
          "application/postscript",
          ".ps",
          "PS",
          "plot",
          []() { return std::make_unique<dc::RendererCairoPs>(); },
          "PostScript (PS)."
        });

        manager.add({
          "eps",
          "application/postscript",
          ".eps",
          "EPS",
          "plot",
          []() { return std::make_unique<dc::RendererCairoEps>(); },
          "Encapsulated PostScript (EPS)."
        });
        
        manager.add({
          "tiff",
          "image/tiff",
          ".tiff",
          "TIFF",
          "plot",
          []() { return std::make_unique<dc::RendererCairoTiff>(); },
          "Tagged Image File Format (TIFF)."
        });
        
#endif
        */


    } // namespace renderers
} // namespace unigd