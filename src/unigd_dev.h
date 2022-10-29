#ifndef __UNIGD_UNIGD_DEV_H__
#define __UNIGD_UNIGD_DEV_H__

#include <compat/optional.hpp>
#include <cpp11/list.hpp>
#include <iostream>
#include <memory>
#include <mutex>

#include "generic_dev.h"
#include "page_store.h"
#include "plot_history.h"
#include "unigd_commons.h"
#include "unigd_external.h"

namespace unigd
{
struct device_params
{
  int bg;
  double width;
  double height;
  double pointsize;
  cpp11::list aliases;
  bool reset_par;
};

class DeviceTarget
{
 public:
  [[nodiscard]] int get_index() const;
  void set_index(int index);
  [[nodiscard]] int get_newest_index() const;
  void set_newest_index(int index);
  [[nodiscard]] bool is_void() const;
  void set_void();

 private:
  int m_index{-1};         // current draw target
  int m_newest_index{-1};  // open draw target
  bool m_void{true};
};

class unigd_device : public generic_dev<unigd_device>
{
 public:
  // Font handling
  cpp11::list system_aliases;
  cpp11::list user_aliases;

  unigd_device(const device_params &t_params);

  unigd_device(const unigd_device &) = delete;
  unigd_device &operator=(unigd_device &) = delete;
  unigd_device &operator=(const unigd_device &) = delete;

  unigd_device(unigd_device &&) = delete;
  unigd_device &operator=(unigd_device &&) = delete;

  bool attach_client(ex::graphics_client *t_client, UNIGD_CLIENT_ID t_client_id,
                     void *t_client_data);
  bool get_client(ex::graphics_client **t_client, UNIGD_CLIENT_ID t_client_id,
                  void **t_client_data);
  bool get_client_anonymous(ex::graphics_client **t_client, void **t_client_data);
  bool remove_client();

  // Synchronous access

  void plt_prerender(int index, double width, double height);
  bool plt_remove(int index);
  bool plt_clear();
  bool plt_render(int index, double width, double height,
                  renderers::render_target *t_renderer, double t_scale);

  // Datastore only access

  ex::device_state plt_state();
  ex::find_results plt_query(int offset, int limit);
  int plt_index(int32_t id);

  // Asynchronous access

  std::unique_ptr<ex::render_data> api_render(ex::renderer_id_t t_renderer_id,
                                              int32_t t_plot_id, double t_width,
                                              double t_height, double t_scale);
  bool api_remove(int32_t t_id);
  bool api_clear();

 protected:
  // Device callbacks

  void dev_activate(pDevDesc dd) override;
  void dev_deactivate(pDevDesc dd) override;
  void dev_close(pDevDesc dd) override;
  void dev_clip(double x0, double x1, double y0, double y1, pDevDesc dd) override;
  void dev_size(double *left, double *right, double *bottom, double *top,
                pDevDesc dd) override;
  void dev_newPage(pGEcontext gc, pDevDesc dd) override;
  void dev_line(double x1, double y1, double x2, double y2, pGEcontext gc,
                pDevDesc dd) override;
  void dev_text(double x, double y, const char *str, double rot, double hadj,
                pGEcontext gc, pDevDesc dd) override;
  double dev_strWidth(const char *str, pGEcontext gc, pDevDesc dd) override;
  void dev_rect(double x0, double y0, double x1, double y1, pGEcontext gc,
                pDevDesc dd) override;
  void dev_circle(double x, double y, double r, pGEcontext gc, pDevDesc dd) override;
  void dev_polygon(int n, double *x, double *y, pGEcontext gc, pDevDesc dd) override;
  void dev_polyline(int n, double *x, double *y, pGEcontext gc, pDevDesc dd) override;
  void dev_path(double *x, double *y, int npoly, int *nper, Rboolean winding,
                pGEcontext gc, pDevDesc dd) override;
  void dev_mode(int mode, pDevDesc dd) override;
  void dev_metricInfo(int c, pGEcontext gc, double *ascent, double *descent,
                      double *width, pDevDesc dd) override;
  void dev_raster(unsigned int *raster, int w, int h, double x, double y, double width,
                  double height, double rot, Rboolean interpolate, pGEcontext gc,
                  pDevDesc dd) override;

 private:
  PlotHistory m_history;
  std::shared_ptr<page_store> m_data_store;

  ex::graphics_client *m_client{nullptr};
  UNIGD_CLIENT_ID m_client_id = 0;
  void *m_client_data{nullptr};

  bool replaying{false};  // Is the device replaying
  DeviceTarget m_target;

  bool m_initialized{false};

  void put(std::unique_ptr<renderers::DrawCall> &&t_dc);

  // set device size
  void resize_device_to_page(pDevDesc dd);

  // graphical parameters for reseting
  cpp11::list m_reset_par;

  std::vector<std::unique_ptr<unigd::renderers::DrawCall>> m_dc_buffer{};
};

}  // namespace unigd

#endif /* __UNIGD_UNIGD_DEV_H__ */
