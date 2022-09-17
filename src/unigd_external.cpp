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
            return {state, static_cast<plot_index_t>(ids.size()), &ids[0]};
        }

        int api_test_fun()
        {
            return 7;
        }

        void api_log(const char *t_message)
        {
            std::string msg(t_message);
            async::r_thread([=]()
                            { Rprintf("unigd client: %s\n", msg.c_str()); });
        }

        UNIGD_CLIENT_ID api_register_client_id()
        {
            static UNIGD_CLIENT_ID client_id_counter = 0;
            return client_id_counter++; // todo: handle overflow
        }

        UNIGD_HANDLE api_device_attach(int devnum, unigd_graphics_client *client, UNIGD_CLIENT_ID client_id, void *client_data)
        {
            auto dev = unigd_device::from_device_number(devnum);
            if (!dev)
            {
                return nullptr;
            }
            if (dev->attach_client(client, client_id, client_data))
            {
                return new unigd_handle_t{dev};
            }
            return nullptr;
        }

        void *api_device_get(int devnum, UNIGD_CLIENT_ID client_id)
        {
            auto dev = unigd_device::from_device_number(devnum);
            if (!dev)
            {
                return nullptr;
            }
            graphics_client *client;
            void *client_data;
            if (!dev->get_client(&client, client_id, &client_data)) {
                return nullptr;
            }

            return client_data;
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

        bool api_plots_clear(UNIGD_HANDLE ugd_handle)
        {
            const auto ugd = static_cast<unigd_handle_t *>(ugd_handle);
            return ugd->device->api_clear();
        }

        bool api_plots_remove(UNIGD_HANDLE ugd_handle, UNIGD_PLOT_ID id)
        {
            const auto ugd = static_cast<unigd_handle_t *>(ugd_handle);
            return ugd->device->api_remove(id);
        }

        UNIGD_RENDER_HANDLE api_render_create(UNIGD_HANDLE ugd_handle, UNIGD_RENDERER_ID renderer_id, UNIGD_PLOT_ID plot_id, unigd_render_args render_args, unigd_render_access *render_access)
        {
            const auto ugd = static_cast<unigd_handle_t *>(ugd_handle);
            auto handle = ugd->device->api_render(renderer_id, plot_id, render_args.width, render_args.height, render_args.scale).release();
            if (handle) {
                size_t buf_size;
                handle->get_data(&render_access->buffer, &buf_size);
                render_access->size = buf_size;
            } else {
                render_access->buffer = nullptr;
                render_access->size = 0;
            }
            return handle;
        }

        void api_render_destroy(UNIGD_RENDER_HANDLE handle)
        {
            delete static_cast<unigd::ex::render_data *>(handle);
        }

        UNIGD_FIND_HANDLE api_plots_find(UNIGD_HANDLE ugd_handle, UNIGD_PLOT_RELATIVE offset, UNIGD_PLOT_INDEX limit, unigd_find_results *results)
        {
            const auto ugd = static_cast<unigd_handle_t *>(ugd_handle);

            auto *re = new find_results{};
            *re = ugd->device->plt_query(offset, limit);
            *results = re->c_repr();
            return re;
        }

        void api_plots_find_destroy(UNIGD_FIND_HANDLE handle)
        {
            delete static_cast<unigd::ex::find_results *>(handle);
        }

        UNIGD_RENDERERS_ENTRY_HANDLE api_renderers_find(UNIGD_RENDERER_ID id, unigd_renderer_info *renderer)
        {
            if (!renderers::find_info(id, renderer))
            {
                return nullptr;
            }
            static int ok_return = 1;
            return static_cast<void *>(&ok_return);
        }

        void api_renderers_find_destroy(UNIGD_RENDERERS_ENTRY_HANDLE handle) {
            // Placeholder in case a renderer lookup ever needs to alloc (e.g. for dynamic renderer adding/removing)
        }

        UNIGD_RENDERERS_HANDLE api_renderers(unigd_renderers_list *renderer) {
            const auto rs = renderers::renderers();

            auto *re = new std::vector<unigd_renderer_info>;

            re->reserve(rs->size());

            for (auto& it: *rs) {
                re->emplace_back(it.second.info);
            }

            *renderer = { &((*re)[0]), re->size() };

            return re;
        }

        void api_renderers_destroy(UNIGD_RENDERERS_HANDLE handle) {
            delete static_cast<std::vector<unigd_renderer_info> *>(handle);
        }

        int api_v1_create(unigd_api_v1 **api_)
        {
            auto api = new unigd_api_v1();

            api->log = api_log;

            api->register_client_id = api_register_client_id;

            api->device_attach = api_device_attach;
            api->device_get = api_device_get;
            api->device_destroy = api_device_destroy;

            api->device_state = api_device_state;

            api->device_plots_clear = api_plots_clear;
            api->device_plots_remove = api_plots_remove;

            api->device_render_create = api_render_create;
            api->device_render_destroy = api_render_destroy;

            api->device_plots_find = api_plots_find;
            api->device_plots_find_destroy = api_plots_find_destroy;

            api->renderers = api_renderers;
            api->renderers_destroy = api_renderers_destroy;
            api->renderers_find = api_renderers_find;
            api->renderers_find_destroy = api_renderers_find_destroy;

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

// There is a bug in cpp11 / decor where pointer types are not detected when the asterisk is right-aligned
// see: https://github.com/r-lib/decor/pull/11
// clang-format off
[[cpp11::init]] void export_api(DllInfo* dll)
// clang-format on
{
    R_RegisterCCallable("unigd", "api_v1_create", reinterpret_cast<DL_FUNC>(unigd::ex::api_v1_create));
    R_RegisterCCallable("unigd", "api_v1_destroy", reinterpret_cast<DL_FUNC>(unigd::ex::api_v1_destroy));
}
