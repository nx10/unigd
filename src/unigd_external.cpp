#include "unigd_dev.h"
#include "renderers.h"
#include "r_thread.h"

#include "unigd_external.h"
#include "unigd_dev.h"

namespace unigd
{
    namespace ex
    {
        using unigd_handle_t = unigd_handle<unigd::unigd_device>;

        unigd_find_results find_results::c_repr()
        {
            return {state, ids.size(), &ids[0]};
        }


        int api_test_fun()
        {
            return 7;
        }


        /*bool attach_client(int devnum, const std::shared_ptr<unigd::graphics_client>& t_client)
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
        }*/

        void api_log(const char *t_message)
        {
            std::string msg(t_message);
            async::r_thread([=]() { Rprintf("unigd client: %s\n", msg.c_str()); });
        }

        UNIGD_HANDLE api_device_attach(int devnum, unigd_graphics_client *client)
        {
            auto dev = unigd_device::from_device_number(devnum);
            if (!dev)
            {
                return nullptr;
            }
            if (dev->attach_client(client))
            {
                return new unigd_handle_t{dev};
            }
            return nullptr;
        }
        void api_device_destroy(UNIGD_HANDLE handle)
        {
            delete static_cast<unigd_handle_t *>(handle);
        }

        unigd_device_state api_device_state(UNIGD_HANDLE ugd_handle)
        {
            const auto ugd = static_cast<unigd_handle_t *>(ugd_handle);
            return ugd->device->plt_state();
        }

        int api_v1_create(unigd_api_v1 **api_)
        {
            auto api = new unigd_api_v1();
            //api->test_fun = test_fun;
            api->log = api_log;
            api->device_attach = api_device_attach;
            api->device_destroy = api_device_destroy;
            api->device_state = api_device_state;

            *api_ = api;

            return 0;
        }

        int api_v1_destroy(unigd_api_v1 *api_)
        {
            delete api_;

          return 0;
        }


    } // namespace ex
} // namespace unigd


[[cpp11::init]]
void export_api(DllInfo* dll)
{
    R_RegisterCCallable("unigd", "api_v1_create", reinterpret_cast<DL_FUNC>(unigd::ex::api_v1_create));
    R_RegisterCCallable("unigd", "api_v1_destroy", reinterpret_cast<DL_FUNC>(unigd::ex::api_v1_destroy));
}
