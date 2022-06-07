// 
// clang-format off

#ifndef UNIGD_EXTERNAL_API_IMPL_H
#define UNIGD_EXTERNAL_API_IMPL_H

#define R_NO_REMAP
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#include <unigd_api.h>
#include <unigd_api_version.h>

namespace unigd
{
    namespace
    {
        int(*pcallable_test_fun)() = NULL;
        bool(*pcallable_attach_client)(int, const std::shared_ptr<unigd::graphics_client>&) = NULL;
        bool(*pcallable_get_client)(int, std::shared_ptr<unigd::graphics_client>*) = NULL;
        bool(*pcallable_get_renderer_list)(std::vector<unigd::renderer_info>*) = NULL;
        bool(*pcallable_get_renderer_info)(const unigd::renderer_id_t&, unigd::renderer_info*) = NULL;
        void(*pcallable_log)(const std::string&) = NULL;
    }

    bool load_api()
    {
        static int(*pcallable_api_version)() = (int(*)()) R_GetCCallable("unigd", "_ccall_api_version");
        if (pcallable_api_version() != UNIGD_EXTERNAL_API_VERSION)
        {
            return false;
        }
        pcallable_test_fun = (int(*)()) R_GetCCallable("unigd", "_ccall_test_fun");
        pcallable_attach_client = (bool(*)(int, const std::shared_ptr<unigd::graphics_client>&)) R_GetCCallable("unigd", "_ccall_attach_client");
        pcallable_get_client = (bool(*)(int, std::shared_ptr<unigd::graphics_client>*)) R_GetCCallable("unigd", "_ccall_get_client");
        pcallable_get_renderer_list = (bool(*)(std::vector<unigd::renderer_info>*)) R_GetCCallable("unigd", "_ccall_get_renderer_list");
        pcallable_get_renderer_info = (bool(*)(const unigd::renderer_id_t&, unigd::renderer_info*)) R_GetCCallable("unigd", "_ccall_get_renderer_info");
        pcallable_log = (void(*)(const std::string&)) R_GetCCallable("unigd", "_ccall_log");
        return true;
    }

    int test_fun()
    {
        return pcallable_test_fun();
    }

    bool attach_client(int devnum, const std::shared_ptr<unigd::graphics_client>& t_client)
    {
        return pcallable_attach_client(devnum, t_client);
    }
    
    bool get_client(int devnum, std::shared_ptr<unigd::graphics_client>* t_client)
    {
        return pcallable_get_client(devnum, t_client);
    }

    bool get_renderer_list(std::vector<unigd::renderer_info>* renderer)
    {
        return pcallable_get_renderer_list(renderer);
    }

    bool get_renderer_info(const unigd::renderer_id_t& id, unigd::renderer_info* renderer)
    {
        return pcallable_get_renderer_info(id, renderer);
    }

    void log(const std::string& t_message)
    {
        pcallable_log(t_message);
    }
    
} // namespace unigd

#endif // UNIGD_EXTERNAL_API_IMPL_H
