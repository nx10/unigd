// 
// clang-format off

#ifndef UNIGD_EXTERNAL_API_H
#define UNIGD_EXTERNAL_API_H

#include <unigd_api/client.h>
#include <unigd_api/device.h>
#include <memory>
#include <vector>

namespace unigd
{
    bool load_api();

    int test_fun();
    bool attach_client(int devnum, const std::shared_ptr<unigd::graphics_client>& t_client);
    bool get_client(int devnum, std::shared_ptr<unigd::graphics_client> *t_client);
    bool get_renderer_list(std::vector<unigd::renderer_info>* renderer);
    bool get_renderer_info(const unigd::renderer_id_t& id, unigd::renderer_info* renderer);
    void log(const std::string &t_message);

} // namespace unigd

#endif // UNIGD_EXTERNAL_API_H
