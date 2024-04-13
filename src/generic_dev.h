#ifndef __UNIGD_GENERIC_DEV_H__
#define __UNIGD_GENERIC_DEV_H__

#define R_NO_REMAP
#include <R_ext/GraphicsEngine.h>

#include <cpp11/R.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/list.hpp>
#include <memory>

namespace unigd
{

template <class T>
class generic_dev : public std::enable_shared_from_this<generic_dev<T>>
{
  struct device_container
  {
    std::shared_ptr<generic_dev<T>> device;
  };

 public:
  generic_dev(double t_width, double t_height, double t_pointsize, int t_fill)
      : m_initial_width(t_width),
        m_initial_height(t_height),
        m_initial_pointsize(t_pointsize),
        m_initial_fill(t_fill)
  {
  }
  virtual ~generic_dev() = default;

  int create(const char *t_device_name)
  {
    auto *container = new device_container{this->shared_from_this()};
    int devnum = -1;

    R_GE_checkVersionOrDie(R_GE_version);
    R_CheckDeviceAvailable();

    BEGIN_SUSPEND_INTERRUPTS
    {
      pDevDesc dd = setup(container);
      if (dd == nullptr)
      {
        cpp11::stop("Failed to start device");
      }

      pGEDevDesc gdd = GEcreateDevDesc(dd);
      GEaddDevice2(gdd, t_device_name);
      GEinitDisplayList(gdd);
      devnum = GEdeviceNumber(gdd);
    }
    END_SUSPEND_INTERRUPTS;

    return devnum + 1;
  }

  // Unsafe direct access. Caller must ensure that dd is from the correct device.
  static inline generic_dev<T> *from_dd(pDevDesc dd)
  {
    return static_cast<device_container *>(dd->deviceSpecific)->device.get();
  }

  // Caller must ensure that dd is from the correct device type.
  // Will return nullptr for invalid device numbers.
  static inline std::shared_ptr<generic_dev<T>> generic_from_device_number(int devnum)
  {
    if (devnum < 1 || devnum > 64)  // R_MaxDevices
    {
      return nullptr;
    }
    const pGEDevDesc gdd = GEgetDevice(devnum - 1);
    if (!gdd)
    {
      return nullptr;
    }
    const pDevDesc dd = gdd->dev;
    if (!dd)
    {
      return nullptr;
    }
    const auto *dev = static_cast<device_container *>(dd->deviceSpecific);
    if (!dev)
    {
      return nullptr;
    }
    return dev->device;
  }

  static inline std::shared_ptr<T> from_device_number(int devnum)
  {
    return std::static_pointer_cast<T>(generic_from_device_number(devnum));
  }

  std::shared_ptr<generic_dev> getptr() { return this->shared_from_this(); }

  static int make_device(const char *t_device_name, generic_dev *t_dev);

  // avoid when possible
  static pDevDesc get_active_pDevDesc()
  {
    pGEDevDesc gdd = GEcurrentDevice();
    if (gdd == nullptr) cpp11::stop("Current device not found");
    pDevDesc dd = gdd->dev;
    if (dd == nullptr) cpp11::stop("Current device not found");
    return dd;
  }

  // protected:
  //  DEVICE CALLBACKS

  // Called when the device becomes the active device
  virtual void dev_activate(pDevDesc dd) {}
  // Called when another device becomes the active device
  virtual void dev_deactivate(pDevDesc dd) {}
  // Called when the device is closed (Object will be destroyed afterwards)
  virtual void dev_close(pDevDesc dd) {}
  // Clip draw area
  virtual void dev_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {}
  // Get the size of the graphics device
  virtual void dev_size(double *left, double *right, double *bottom, double *top,
                        pDevDesc dd)
  {
  }
  // Start a new page
  virtual void dev_newPage(pGEcontext gc, pDevDesc dd) {}
  // Draw line
  virtual void dev_line(double x1, double y1, double x2, double y2, pGEcontext gc,
                        pDevDesc dd)
  {
  }
  // Draw text
  virtual void dev_text(double x, double y, const char *str, double rot, double hadj,
                        pGEcontext gc, pDevDesc dd)
  {
  }
  // Get String width
  virtual double dev_strWidth(const char *str, pGEcontext gc, pDevDesc dd) { return 0; }
  // Draw rectangle
  virtual void dev_rect(double x0, double y0, double x1, double y1, pGEcontext gc,
                        pDevDesc dd)
  {
  }
  // Draw circle
  virtual void dev_circle(double x, double y, double r, pGEcontext gc, pDevDesc dd) {}
  // Draw polygon
  virtual void dev_polygon(int n, double *x, double *y, pGEcontext gc, pDevDesc dd) {}
  // Draw polyline
  virtual void dev_polyline(int n, double *x, double *y, pGEcontext gc, pDevDesc dd) {}
  // Draw path
  virtual void dev_path(double *x, double *y, int npoly, int *nper, Rboolean winding,
                        pGEcontext gc, pDevDesc dd)
  {
  }
  // start draw mode = 1, stop draw mode = 0
  virtual void dev_mode(int mode, pDevDesc dd) {}
  // Get singe char font metrics
  virtual void dev_metricInfo(int c, pGEcontext gc, double *ascent, double *descent,
                              double *width, pDevDesc dd)
  {
  }
  // Integer matrix (R colors)
  virtual SEXP dev_cap(pDevDesc dd) { return R_NilValue; }
  // Draw raster image
  virtual void dev_raster(unsigned int *raster, int w, int h, double x, double y,
                          double width, double height, double rot, Rboolean interpolate,
                          pGEcontext gc, pDevDesc dd)
  {
  }

  // R_GE_version >= 13
  virtual SEXP dev_setPattern(SEXP pattern, pDevDesc dd) { return R_NilValue; }
  virtual void dev_releasePattern(SEXP ref, pDevDesc dd) {}
  virtual SEXP dev_setClipPath(SEXP path, SEXP ref, pDevDesc dd) { return R_NilValue; }
  virtual void dev_releaseClipPath(SEXP ref, pDevDesc dd) {}
  virtual SEXP dev_setMask(SEXP path, SEXP ref, pDevDesc dd) { return R_NilValue; }
  virtual void dev_releaseMask(SEXP ref, pDevDesc dd) {}

  // R_GE_version >= 15
  virtual SEXP dev_defineGroup(SEXP source, int op, SEXP destination, pDevDesc dd)
  {
    return R_NilValue;
  }
  virtual void dev_useGroup(SEXP ref, SEXP trans, pDevDesc dd) {}
  virtual void dev_releaseGroup(SEXP ref, pDevDesc dd) {}
  virtual void dev_stroke(SEXP path, const pGEcontext gc, pDevDesc dd) {}
  virtual void dev_fill(SEXP path, int rule, const pGEcontext gc, pDevDesc dd) {}
  virtual void dev_fillStroke(SEXP path, int rule, const pGEcontext gc, pDevDesc dd) {}

  // R_GE_version >= 16
  virtual void dev_glyph(int n, int *glyphs, double *x, double *y, SEXP font,
                         double size, int colour, double rot, pDevDesc dd) {};

  // GRAPHICS DEVICE FEATURE FLAGS

  bool m_df_cap = false;
  bool m_df_displaylist = false;

  // INITIAL VALUES

  const double m_initial_width;
  const double m_initial_height;
  const double m_initial_pointsize;
  const int m_initial_fill = R_RGB(255, 255, 255);
  const int m_initial_col = R_RGB(0, 0, 0);

 private:
  pDevDesc setup(device_container *t_device_specific)
  {
    pDevDesc dd = (DevDesc *)calloc(1, sizeof(DevDesc));
    if (dd == nullptr) return dd;

    dd->startfill = m_initial_fill;
    dd->startcol = m_initial_col;
    dd->startps = m_initial_pointsize;
    dd->startlty = 0;
    dd->startfont = 1;
    dd->startgamma = 1;

    // Callbacks
    dd->activate = [](pDevDesc dd) { from_dd(dd)->dev_activate(dd); };
    dd->deactivate = [](pDevDesc dd) { from_dd(dd)->dev_deactivate(dd); };
    dd->close = [](pDevDesc dd)
    {
      auto *container = static_cast<device_container *>(dd->deviceSpecific);
      container->device->dev_close(dd);
      delete container;
    };
    dd->clip = [](double x0, double x1, double y0, double y1, pDevDesc dd)
    { from_dd(dd)->dev_clip(x0, x1, y0, y1, dd); };
    dd->size = [](double *left, double *right, double *bottom, double *top, pDevDesc dd)
    {
      *left = dd->left;
      *top = dd->top;
      *right = dd->right;
      *bottom = dd->bottom;

      from_dd(dd)->dev_size(left, right, bottom, top, dd);
    };
    dd->newPage = [](pGEcontext gc, pDevDesc dd) { from_dd(dd)->dev_newPage(gc, dd); };
    dd->line = [](double x1, double y1, double x2, double y2, pGEcontext gc, pDevDesc dd)
    { from_dd(dd)->dev_line(x1, y1, x2, y2, gc, dd); };
    dd->text = [](double x, double y, const char *str, double rot, double hadj,
                  pGEcontext gc, pDevDesc dd)
    { from_dd(dd)->dev_text(x, y, str, rot, hadj, gc, dd); };
    dd->strWidth = [](const char *str, pGEcontext gc, pDevDesc dd)
    { return from_dd(dd)->dev_strWidth(str, gc, dd); };
    dd->rect = [](double x0, double y0, double x1, double y1, pGEcontext gc, pDevDesc dd)
    { from_dd(dd)->dev_rect(x0, y0, x1, y1, gc, dd); };
    dd->circle = [](double x, double y, double r, pGEcontext gc, pDevDesc dd)
    { from_dd(dd)->dev_circle(x, y, r, gc, dd); };
    dd->polygon = [](int n, double *x, double *y, pGEcontext gc, pDevDesc dd)
    { from_dd(dd)->dev_polygon(n, x, y, gc, dd); };
    dd->polyline = [](int n, double *x, double *y, pGEcontext gc, pDevDesc dd)
    { from_dd(dd)->dev_polyline(n, x, y, gc, dd); };
    dd->path = [](double *x, double *y, int npoly, int *nper, Rboolean winding,
                  pGEcontext gc, pDevDesc dd)
    { from_dd(dd)->dev_path(x, y, npoly, nper, winding, gc, dd); };
    dd->mode = [](int mode, pDevDesc dd) { from_dd(dd)->dev_mode(mode, dd); };
    dd->metricInfo = [](int c, pGEcontext gc, double *ascent, double *descent,
                        double *width, pDevDesc dd)
    { from_dd(dd)->dev_metricInfo(c, gc, ascent, descent, width, dd); };
    dd->raster = [](unsigned int *raster, int w, int h, double x, double y, double width,
                    double height, double rot, Rboolean interpolate, pGEcontext gc,
                    pDevDesc dd) {
      from_dd(dd)->dev_raster(raster, w, h, x, y, width, height, rot, interpolate, gc,
                              dd);
    };
#if R_GE_version >= 13
    dd->setPattern = [](SEXP pattern, pDevDesc dd)
    { return from_dd(dd)->dev_setPattern(pattern, dd); };
    dd->releasePattern = [](SEXP ref, pDevDesc dd)
    { from_dd(dd)->dev_releasePattern(ref, dd); };
    dd->setClipPath = [](SEXP path, SEXP ref, pDevDesc dd)
    { return from_dd(dd)->dev_setClipPath(path, ref, dd); };
    dd->releaseClipPath = [](SEXP ref, pDevDesc dd)
    { from_dd(dd)->dev_releaseClipPath(ref, dd); };
    dd->setMask = [](SEXP path, SEXP ref, pDevDesc dd)
    { return from_dd(dd)->dev_setMask(path, ref, dd); };
    dd->releaseMask = [](SEXP ref, pDevDesc dd)
    { from_dd(dd)->dev_releaseMask(ref, dd); };
#endif
#if R_GE_version >= 15
    dd->defineGroup = [](SEXP source, int op, SEXP destination, pDevDesc dd)
    { return from_dd(dd)->dev_defineGroup(source, op, destination, dd); };
    dd->useGroup = [](SEXP ref, SEXP trans, pDevDesc dd)
    { return from_dd(dd)->dev_useGroup(ref, trans, dd); };
    dd->releaseGroup = [](SEXP ref, pDevDesc dd)
    { return from_dd(dd)->dev_releaseGroup(ref, dd); };
    dd->stroke = [](SEXP path, pGEcontext gc, pDevDesc dd)
    { return from_dd(dd)->dev_stroke(path, gc, dd); };
    dd->fill = [](SEXP path, int rule, pGEcontext gc, pDevDesc dd)
    { return from_dd(dd)->dev_fill(path, rule, gc, dd); };
    dd->fillStroke = [](SEXP path, int rule, pGEcontext gc, pDevDesc dd)
    { return from_dd(dd)->dev_fillStroke(path, rule, gc, dd); };

    // From "SEXP devcap(SEXP args)" in "grDevices/src/devices.c" it seems
    // that this function may be used to set some entries to the capability list.
    // Must return a list of the correct length and only integers.
    // ugd(); dev.capabilities()
    dd->capabilities = [](SEXP t_cap)
    {
      auto cap = cpp11::writable::list(t_cap);
      cap[R_GE_capability_patterns] = cpp11::writable::integers({0});
      cap[R_GE_capability_clippingPaths] = cpp11::writable::integers({0});
      cap[R_GE_capability_masks] = cpp11::writable::integers({0});
      cap[R_GE_capability_compositing] = cpp11::writable::integers({0});
      cap[R_GE_capability_transformations] = cpp11::writable::integers({0});
      cap[R_GE_capability_paths] = cpp11::writable::integers({0});
      #if R_GE_version >= 16
      cap[R_GE_capability_glyphs] = cpp11::writable::integers({0});
      #endif
      return cpp11::as_sexp(cap);
    };
#endif
#if R_GE_version >= 16
    dd->glyph = [](int n, int *glyphs, double *x, double *y, SEXP font,
                   double size, int colour, double rot, pDevDesc dd)
    { from_dd(dd)->dev_glyph(n, glyphs, x, y, font, size, colour, rot, dd); };
#endif

    if (m_df_cap)
    {
      dd->cap = [](pDevDesc dd) { return from_dd(dd)->dev_cap(dd); };
    }
    else
    {
      dd->cap = nullptr;
    }

    // UTF-8 support
    dd->wantSymbolUTF8 = static_cast<Rboolean>(1);
    dd->hasTextUTF8 = static_cast<Rboolean>(1);
    dd->textUTF8 = dd->text;
    dd->strWidthUTF8 = dd->strWidth;

    // Screen Dimensions in pts
    dd->left = 0;
    dd->top = 0;
    dd->right = m_initial_width;
    dd->bottom = m_initial_height;

    // Magic constants copied from other graphics devices
    // nominal character sizes in pts
    dd->cra[0] = 0.9 * m_initial_pointsize;
    dd->cra[1] = 1.2 * m_initial_pointsize;
    // character alignment offsets
    dd->xCharOffset = 0.4900;
    dd->yCharOffset = 0.3333;
    dd->yLineBias = 0.2;
    // inches per pt
    dd->ipr[0] = 1.0 / 72.0;
    dd->ipr[1] = 1.0 / 72.0;

    // Capabilities
    dd->canClip = static_cast<Rboolean>(1);
    dd->canHAdj = 1;
    dd->canChangeGamma = static_cast<Rboolean>(0);
    dd->displayListOn = static_cast<Rboolean>(m_df_displaylist);
    dd->haveTransparency = 2;
    dd->haveTransparentBg = 3;

    dd->canGenMouseDown = static_cast<Rboolean>(0);
    dd->canGenMouseMove = static_cast<Rboolean>(0);
    dd->canGenMouseUp = static_cast<Rboolean>(0);
    dd->canGenKeybd = static_cast<Rboolean>(0);
#if R_GE_version >= 12
    dd->canGenIdle = static_cast<Rboolean>(0);
#endif
    dd->gettingEvent = static_cast<Rboolean>(0);

    dd->haveRaster = 2;
    dd->haveCapture = 1;
    dd->haveLocator = 1;

    dd->newFrameConfirm = nullptr;
    dd->onExit = nullptr;
    dd->eventEnv = R_NilValue;
    dd->eventHelper = nullptr;
    dd->holdflush = nullptr;

#if R_GE_version >= 14
    dd->deviceClip = static_cast<Rboolean>(0);
#endif

    // Set maximum version
#if R_GE_version == 13
    dd->deviceVersion = R_GE_definitions;
#endif
#if R_GE_version == 14
    dd->deviceVersion = R_GE_deviceClip;
#endif
#if R_GE_version == 15
    dd->deviceVersion = R_GE_group;
#endif
#if R_GE_version >= 16
    dd->deviceVersion = R_GE_glyphs;
#endif

    // Device specific
    dd->deviceSpecific = t_device_specific;
    return dd;
  }
};
}  // namespace unigd

#endif /* __UNIGD_GENERIC_DEV_H__ */
