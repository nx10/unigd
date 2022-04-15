#ifndef RENDERER_MANAGER_H
#define RENDERER_MANAGER_H

#include <unigd_api/draw_data.h>

#include <functional>
#include <string>
#include <unordered_map>

namespace unigd
{
    namespace renderers
    {
        struct renderer_info
        {
            std::string id;
            std::string mime;
            std::string fileext;
            std::string name;
            std::string type;
            std::string description;
            bool text;
        };
        
        struct renderer_gen
        {
            renderer_info info;
            std::function<std::unique_ptr<dc::Renderer>()> renderer;
        };

    } // namespace renderers

} // namespace unigd

#endif // RENDERER_MANAGER_H