#ifndef __UNIGD_UNIGD_EXTERNAL_H__
#define __UNIGD_UNIGD_EXTERNAL_H__

#include <stdlib.h>
#include <unigd_api_v1.h>

#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace unigd
{
namespace ex
{
using plot_index_t = UNIGD_PLOT_INDEX;
using plot_relative_t = UNIGD_PLOT_RELATIVE;
using plot_id_t = UNIGD_PLOT_ID;
using renderer_id_t = UNIGD_RENDERER_ID;

using graphics_client = unigd_graphics_client;

inline char *copy_c_str(std::string str)
{
  const auto size = str.size();
  char *buffer = new char[size + 1];
  memcpy(buffer, str.c_str(), size + 1);
  return buffer;
}

inline void destroy_c_str(char *str) { delete str; }

template <class DeviceClass>
struct unigd_handle
{
  std::shared_ptr<DeviceClass> device;
};

using device_state = unigd_device_state;

struct find_results
{
  unigd_device_state state;
  std::vector<plot_id_t> ids;

  unigd_find_results c_repr();
};

class render_data
{
 public:
  render_data() = default;
  virtual ~render_data() = default;

  virtual void get_data(const uint8_t **t_buf, size_t *t_size) const = 0;
};
}  // namespace ex

}  // namespace unigd

#endif /* __UNIGD_UNIGD_EXTERNAL_H__ */
