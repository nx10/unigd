#ifndef UNIGD_RENDERERS_H
#define UNIGD_RENDERERS_H

#include <unigd_api/renderers_types.h>

namespace unigd
{
    namespace renderers
    {
        bool find_renderer(const std::string &id, const renderer_gen **renderer);
        bool find_info(const std::string &id, const renderer_info **renderer);
        const std::unordered_map<std::string, renderer_gen> *renderers();
    } // namespace renderers
    
} // namespace unigd


#endif // UNIGD_RENDERERS_H