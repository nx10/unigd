#ifndef RENDERER_MANAGER_H
#define RENDERER_MANAGER_H

#include <unigd_api/draw_data.h>

#include <compat/optional.hpp>
#include <functional>
#include <string>
#include <unordered_map>

namespace unigd
{

    template <typename T>
    struct RendererManagerInfo
    {
        std::string id;
        std::string mime;
        std::string fileext;
        std::string name;
        std::string type;
        std::function<std::unique_ptr<T>()> renderer;
        std::string description;
    };
    
    using StringRendererInfo = RendererManagerInfo<dc::StringRenderingTarget>;
    using BinaryRendererInfo = RendererManagerInfo<dc::BinaryRenderingTarget>;

    class RendererManager
    {
    public:
        static const RendererManager &defaults();

        [[nodiscard]] const std::unordered_map<std::string, StringRendererInfo> &string_renderers() const;
        [[nodiscard]] const std::unordered_map<std::string, BinaryRendererInfo> &binary_renderers() const;

        void add(const StringRendererInfo &renderer);
        void add(const BinaryRendererInfo &renderer);
        
        std::experimental::optional<const StringRendererInfo &> find_string(const std::string &id) const;
        std::experimental::optional<const BinaryRendererInfo &> find_binary(const std::string &id) const;

        std::size_t size() const;

    private:
        static const RendererManager generate_default();
        static const RendererManager defaults_;
        std::unordered_map<std::string, StringRendererInfo> m_string_renderers;
        std::unordered_map<std::string, BinaryRendererInfo> m_binary_renderers;
    };

} // namespace unigd

#endif // RENDERER_MANAGER_H