#ifndef UNIGD_RENDERERS_H
#define UNIGD_RENDERERS_H

#include "draw_data.h"
#include <unigd_api/device.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace unigd
{
    namespace renderers
    {
        
        class Renderer : public render_data
        {
        public:
            Renderer() = default;
            virtual ~Renderer() = default;

            virtual void render(const Page &t_page, double t_scale) = 0;
            virtual void get_data(const uint8_t **t_buf, size_t *t_size) const = 0;
        };

        using renderer_gen = std::function<std::unique_ptr<Renderer> ()>;
        struct renderer_map_entry
        {
            renderer_info info;
            renderer_gen generator;
        };

        bool find(const std::string &id, const renderer_map_entry **renderer);
        bool find_generator(const std::string &id, const renderer_gen **renderer);
        bool find_info(const std::string &id, const renderer_info **renderer);
        const std::unordered_map<std::string, renderer_map_entry> *renderers();
    } // namespace renderers
    
} // namespace unigd


#endif // UNIGD_RENDERERS_H