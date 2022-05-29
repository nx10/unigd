#include <unigd_api/client.h>
#include "unigd_dev.h"
#include "renderers.h"
#include "unigd_dev.h"
#include "unigd_api_version.h"

namespace unigd
{
    namespace external
    {
        int api_version()
        {
            return UNIGD_EXTERNAL_API_VERSION;
        }


        int test_fun()
        {
            return 7;
        }


        bool attach_client(int devnum, const std::shared_ptr<unigd::graphics_client>& t_client)
        {
            auto dev = validate_device<unigd_device>(devnum);
            if (!dev)
            {
                return false;
            }
            return dev->attach_client(t_client);
        }

        bool get_client(int devnum, std::shared_ptr<unigd::graphics_client>* t_client)
        {
            auto dev = validate_device<unigd_device>(devnum);
            if (!dev)
            {
                return false;
            }
            return dev->get_client(t_client);
        }

        bool get_renderer_list(std::vector<unigd::renderer_info> *renderer)
        {
            const auto renderer_map = unigd::renderers::renderers();
            renderer->reserve(renderer_map->size());
            for (auto& map_entry: *renderer_map) {
                renderer->push_back(map_entry.second.info);
            }
            return true;
        }

        bool get_renderer_info(const unigd::renderer_id_t& id, unigd::renderer_info* renderer)
        {
            const unigd::renderer_info *found;
            if (unigd::renderers::find_info(id, &found))
            {
                *renderer = *found;
                return true;
            }
            return false;
        }    
    } // namespace external
} // namespace unigd


[[cpp11::init]]
void export_api(DllInfo* dll) 
{
    R_RegisterCCallable("unigd", "_ccall_api_version", reinterpret_cast<DL_FUNC>(unigd::external::api_version));
    R_RegisterCCallable("unigd", "_ccall_test_fun", reinterpret_cast<DL_FUNC>(unigd::external::test_fun));
    R_RegisterCCallable("unigd", "_ccall_attach_client", reinterpret_cast<DL_FUNC>(unigd::external::attach_client));
    R_RegisterCCallable("unigd", "_ccall_get_client", reinterpret_cast<DL_FUNC>(unigd::external::get_client));
    R_RegisterCCallable("unigd", "_ccall_get_renderer_list", reinterpret_cast<DL_FUNC>(unigd::external::get_renderer_list));
    R_RegisterCCallable("unigd", "_ccall_get_renderer_info", reinterpret_cast<DL_FUNC>(unigd::external::get_renderer_info));
}