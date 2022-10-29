#ifndef __UNIGD_UNIGD_DATA_STORE_H__
#define __UNIGD_UNIGD_DATA_STORE_H__

#include <stdint.h>

#include <atomic>
#include <compat/optional.hpp>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

#include "geom.h"
#include "renderers.h"
#include "unigd_external.h"

namespace unigd
{
class HttpgdDataStore
{
 public:
  HttpgdDataStore() = default;

  HttpgdDataStore(const HttpgdDataStore &) = delete;
  HttpgdDataStore &operator=(HttpgdDataStore &) = delete;
  HttpgdDataStore &operator=(const HttpgdDataStore &) = delete;
  
  HttpgdDataStore(HttpgdDataStore &&) = delete;
  HttpgdDataStore &operator=(HttpgdDataStore &&) = delete;

  std::experimental::optional<ex::plot_index_t> find_index(ex::plot_id_t t_id);
  std::experimental::optional<ex::plot_relative_t> normalize_index(
      ex::plot_relative_t t_index);

  bool render(ex::plot_relative_t t_index, renderers::render_target *t_renderer,
              double t_scale);
  bool render_if_size(ex::plot_relative_t t_index, renderers::render_target *t_renderer,
                      double t_scale, gvertex<double> t_target_size);

  ex::plot_index_t append(gvertex<double> t_size);
  void clear(ex::plot_relative_t t_index, bool t_silent);
  bool remove(ex::plot_relative_t t_index, bool t_silent);
  bool remove_all();
  void resize(ex::plot_relative_t t_index, gvertex<double> t_size);
  gvertex<double> size(ex::plot_relative_t t_index);

  void fill(ex::plot_relative_t t_index, color_t t_fill);
  void add_dc(ex::plot_relative_t t_index, std::unique_ptr<renderers::DrawCall> &&t_dc,
              bool t_silent);
  void add_dc(ex::plot_relative_t t_index,
              std::vector<std::unique_ptr<renderers::DrawCall>> &&t_dcs, bool t_silent);
  void clip(ex::plot_relative_t t_index, grect<double> t_rect);

  ex::device_state state();
  void set_device_active(bool t_active);

  ex::find_results query(ex::plot_relative_t t_offset, ex::plot_index_t t_limit);

  void extra_css(std::experimental::optional<std::string> t_extra_css);

 private:
  std::shared_timed_mutex m_store_mutex;

  ex::plot_id_t m_id_counter = 0;
  std::vector<renderers::Page> m_pages{};
  int m_upid = 0;
  bool m_device_active = true;

  std::experimental::optional<std::string> m_extra_css{};

  void m_inc_upid();

  inline bool m_valid_index(ex::plot_relative_t t_index);
  inline size_t m_index_to_pos(ex::plot_relative_t t_index);
};

}  // namespace unigd

#endif /* __UNIGD_UNIGD_DATA_STORE_H__ */
