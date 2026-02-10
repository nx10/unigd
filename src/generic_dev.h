#ifndef __UNIGD_GENERIC_DEV_H__
#define __UNIGD_GENERIC_DEV_H__

// C++ wrapper around R's graphics device interface (DevDesc / R_ext/GraphicsDevice.h).
//
// Provides a virtual method for each DevDesc callback, so subclasses can
// override only the operations they need. All coordinates are in device units
// (points by default, 72 per inch). Colors are 32-bit ABGR integers
// (see R_RGB, R_RGBA macros).
//
// Usage:
//
//   class my_device : public generic_dev<my_device> {
//     // override dev_line, dev_rect, ... as needed
//   };
//
//   auto dev = std::make_shared<my_device>(width, height, pointsize, fill);
//   dev->create("my_device");
//
// The template parameter T (CRTP) must be the subclass itself. This enables
// type-safe downcasts via from_device_number<T>().
//
// Lifetime: the device must be created as a std::shared_ptr *before* calling
// create(), which calls shared_from_this(). R co-owns the shared_ptr through
// device_container stored in dd->deviceSpecific. The shared_ptr is released
// when R closes the device (dev_close callback).

#define R_NO_REMAP
#include <R_ext/GraphicsEngine.h>
#include <memory>

#include <cpp11/R.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/list.hpp>

namespace unigd
{

template <class T>
class generic_dev : public std::enable_shared_from_this<generic_dev<T>>
{
  // Stored in dd->deviceSpecific. Prevents the device object from being
  // destroyed while R still holds a reference to the DevDesc.
  struct device_container
  {
    std::shared_ptr<generic_dev<T>> device;
  };

 public:
  generic_dev(double t_width, double t_height, double t_pointsize, int t_fill)
      : m_initial_width(t_width)
      , m_initial_height(t_height)
      , m_initial_pointsize(t_pointsize)
      , m_initial_fill(t_fill)
  {
  }

  virtual ~generic_dev() = default;

  // Register this device with R's graphics engine.
  // Must be called on a live shared_ptr (shared_from_this).
  // Returns the 1-based R device number on success.
  int create(const char* t_device_name)
  {
    // These can longjmp on failure, so allocate the container after.
    R_GE_checkVersionOrDie(R_GE_version);
    R_CheckDeviceAvailable();

    auto container =
        std::make_unique<device_container>(device_container{this->shared_from_this()});
    int devnum = -1;

    BEGIN_SUSPEND_INTERRUPTS
    {
      pDevDesc dd = setup(container.get());
      if (dd == nullptr)
      {
        container.reset();  // cpp11::stop longjmps; destructor won't run
        cpp11::stop("Failed to start device");
      }
      container.release();  // R now owns via dd->deviceSpecific

      pGEDevDesc gdd = GEcreateDevDesc(dd);
      GEaddDevice2(gdd, t_device_name);
      GEinitDisplayList(gdd);
      devnum = GEdeviceNumber(gdd);
    }
    END_SUSPEND_INTERRUPTS;

    return devnum + 1;
  }

  // --- Device lookup utilities ---

  // Extract the generic_dev pointer from a DevDesc. No type or ownership
  // checking -- caller must guarantee dd belongs to this device type.
  static inline generic_dev<T>* from_dd(pDevDesc dd)
  {
    return static_cast<device_container*>(dd->deviceSpecific)->device.get();
  }

  // Look up a device by its 1-based R device number (as returned by create()).
  // Returns nullptr if the number is out of range or the device is gone.
  // Caller must ensure the device at that number is actually a generic_dev<T>.
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
    const auto* dev = static_cast<device_container*>(dd->deviceSpecific);
    if (!dev)
    {
      return nullptr;
    }
    return dev->device;
  }

  // Like generic_from_device_number but returns the concrete subclass type T.
  static inline std::shared_ptr<T> from_device_number(int devnum)
  {
    return std::static_pointer_cast<T>(generic_from_device_number(devnum));
  }

  std::shared_ptr<generic_dev> getptr() { return this->shared_from_this(); }

  static int make_device(const char* t_device_name, generic_dev* t_dev);

  // Get the DevDesc of whichever device R currently considers active.
  // Prefer from_device_number() when you know the device number.
  static pDevDesc get_active_pDevDesc()
  {
    pGEDevDesc gdd = GEcurrentDevice();
    if (gdd == nullptr)
    {
      cpp11::stop("Current device not found");
    }
    pDevDesc dd = gdd->dev;
    if (dd == nullptr)
    {
      cpp11::stop("Current device not found");
    }
    return dd;
  }

  // =========================================================================
  //  DEVICE CALLBACKS
  //
  //  Override these virtual methods in your subclass. All have safe no-op
  //  defaults. The wrapper wires each one into the corresponding DevDesc
  //  function pointer via a stateless lambda trampoline in setup().
  //
  //  gc (pGEcontext) carries the current graphics state: col, fill, lwd,
  //  lty, font, cex, ps, etc. dd (pDevDesc) is the low-level device
  //  descriptor -- use it to read/write device dimensions or to access
  //  deviceSpecific data.
  // =========================================================================

  // --- Lifecycle ---

  // The device became the active device (e.g. dev.set()).
  virtual void dev_activate(pDevDesc dd) {}

  // Another device became active.
  virtual void dev_deactivate(pDevDesc dd) {}

  // The device is being closed (dev.off()). Release resources here.
  // The shared_ptr preventing destruction is released immediately after.
  virtual void dev_close(pDevDesc dd) {}

  // --- Drawing primitives ---
  // All coordinates are in device units. Colors come from gc->col (stroke)
  // and gc->fill. NA_INTEGER means "don't draw" / "don't fill".

  // Restrict subsequent drawing to the rectangle (x0,y0)-(x1,y1).
  virtual void dev_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {}

  // Report the current device dimensions. The default values from
  // dd->left/top/right/bottom are pre-filled before this is called;
  // override to report a dynamically resized device.
  virtual void dev_size(double* left, double* right, double* bottom, double* top,
                        pDevDesc dd)
  {
  }

  // Begin a new page. Fill background with gc->fill if opaque.
  virtual void dev_newPage(pGEcontext gc, pDevDesc dd) {}

  virtual void dev_line(double x1, double y1, double x2, double y2, pGEcontext gc,
                        pDevDesc dd)
  {
  }

  // Draw text at (x,y) rotated by rot degrees. hadj is horizontal
  // adjustment: 0 = left-aligned, 0.5 = centered, 1 = right-aligned.
  virtual void dev_text(double x, double y, const char* str, double rot, double hadj,
                        pGEcontext gc, pDevDesc dd)
  {
  }

  // Return the width of str in device units. Used for text layout.
  // Must return 0 if metrics are unavailable.
  virtual double dev_strWidth(const char* str, pGEcontext gc, pDevDesc dd) { return 0; }

  virtual void dev_rect(double x0, double y0, double x1, double y1, pGEcontext gc,
                        pDevDesc dd)
  {
  }

  // r is in x-axis device units (relevant if pixels are non-square).
  virtual void dev_circle(double x, double y, double r, pGEcontext gc, pDevDesc dd) {}

  virtual void dev_polygon(int n, double* x, double* y, pGEcontext gc, pDevDesc dd) {}

  virtual void dev_polyline(int n, double* x, double* y, pGEcontext gc, pDevDesc dd) {}

  // Draw one or more sub-paths as a single compound path.
  // npoly = number of sub-paths, nper[i] = number of points in sub-path i.
  // winding: TRUE = nonzero winding rule, FALSE = even-odd rule.
  virtual void dev_path(double* x, double* y, int npoly, int* nper, Rboolean winding,
                        pGEcontext gc, pDevDesc dd)
  {
  }

  // Called when R starts (mode=1) or stops (mode=0) a batch of drawing
  // operations. Can be used to buffer output.
  virtual void dev_mode(int mode, pDevDesc dd) {}

  // --- Text metrics ---

  // Return ascent, descent, and width for a single character c in device
  // units. In a UTF-8 locale c is a Unicode code point; in an 8-bit locale
  // it is a char. Must set all three to 0 if metrics are unavailable.
  virtual void dev_metricInfo(int c, pGEcontext gc, double* ascent, double* descent,
                              double* width, pDevDesc dd)
  {
  }

  // --- Raster operations ---

  // Capture the current device contents as an integer matrix of R colors
  // (row-major ABGR). Requires m_df_cap = true. Return R_NilValue if
  // capture is not possible.
  virtual SEXP dev_cap(pDevDesc dd) { return R_NilValue; }

  // Draw a raster image. raster is row-major ABGR, w*h pixels.
  // (x,y) is the bottom-left corner, rot is degrees counter-clockwise.
  virtual void dev_raster(unsigned int* raster, int w, int h, double x, double y,
                          double width, double height, double rot, Rboolean interpolate,
                          pGEcontext gc, pDevDesc dd)
  {
  }

  // --- Graphical definitions (R >= 4.1, R_GE_version >= 13) ---
  //
  // Patterns, clipping paths, and masks are R-level objects. The set*
  // methods receive a definition and must return an opaque SEXP reference
  // that the engine will pass back in subsequent drawing calls
  // (via gc->patternFill) and eventually to the matching release* method.
  // Return R_NilValue to indicate the feature is unsupported.
  //
  // Report support levels through dev_capabilities() so R can query them
  // via dev.capabilities().

  virtual SEXP dev_setPattern(SEXP pattern, pDevDesc dd) { return R_NilValue; }

  virtual void dev_releasePattern(SEXP ref, pDevDesc dd) {}

  virtual SEXP dev_setClipPath(SEXP path, SEXP ref, pDevDesc dd) { return R_NilValue; }

  virtual void dev_releaseClipPath(SEXP ref, pDevDesc dd) {}

  virtual SEXP dev_setMask(SEXP path, SEXP ref, pDevDesc dd) { return R_NilValue; }

  virtual void dev_releaseMask(SEXP ref, pDevDesc dd) {}

  // --- Groups and composed paths (R >= 4.2, R_GE_version >= 15) ---
  //
  // Groups allow compositing: source is drawn on top of destination using
  // the Porter-Duff or blend operator op (R_GE_compositeOver, etc.).
  // Return an opaque reference from defineGroup; pass it back to useGroup
  // to render. trans is an affine transformation matrix or R_NilValue.

  virtual SEXP dev_defineGroup(SEXP source, int op, SEXP destination, pDevDesc dd)
  {
    return R_NilValue;
  }

  virtual void dev_useGroup(SEXP ref, SEXP trans, pDevDesc dd) {}

  virtual void dev_releaseGroup(SEXP ref, pDevDesc dd) {}

  // Stroke / fill a path defined by an R drawing function.
  // rule: R_GE_nonZeroWindingRule or R_GE_evenOddRule.
  virtual void dev_stroke(SEXP path, const pGEcontext gc, pDevDesc dd) {}

  virtual void dev_fill(SEXP path, int rule, const pGEcontext gc, pDevDesc dd) {}

  virtual void dev_fillStroke(SEXP path, int rule, const pGEcontext gc, pDevDesc dd) {}

  // --- Advanced typesetting (R >= 4.3, R_GE_version >= 16) ---

  // Render pre-positioned glyphs. glyphs[i] is a glyph ID in the font
  // described by the SEXP font parameter (file path, index, family, etc.).
  virtual void dev_glyph(int n, int* glyphs, double* x, double* y, SEXP font, double size,
                         int colour, double rot, pDevDesc dd)
  {
  }

  // --- Interactive / optional callbacks ---

  // Wait for a mouse click and return the location in device coordinates.
  // Return TRUE if a location was obtained, FALSE to cancel.
  // Only wired if m_df_locator is true.
  virtual Rboolean dev_locator(double* x, double* y, pDevDesc dd)
  {
    return static_cast<Rboolean>(0);
  }

  // Control output buffering. R calls this with increasing level to hold
  // output and level=0 to flush. Return the current hold level.
  // Default returns 0 (always flush immediately).
  virtual int dev_holdflush(pDevDesc dd, int level) { return 0; }

  // Custom new-frame confirmation (e.g. for par(ask=TRUE)).
  // Return TRUE if handled by the device, FALSE to let the engine handle it.
  virtual Rboolean dev_newFrameConfirm(pDevDesc dd) { return static_cast<Rboolean>(0); }

  // Called by GEonExit when R aborts an operation. Use for cleanup that
  // can't wait for dev_close.
  virtual void dev_onExit(pDevDesc dd) {}

  // Called during getGraphicsEvent() processing.
  // code: 1 = start, 2 = poll, 0 = done.
  virtual void dev_eventHelper(pDevDesc dd, int code) {}

  // --- Device capabilities (R >= 4.2, R_GE_version >= 15) ---
  //
  // Called by dev.capabilities(). Receives a pre-allocated list and must
  // fill in integer values for each capability index. Values: 0 = no
  // support (or NA), 1+ = supported (meaning varies per capability).
  //
  // Note: the R API for this callback does not pass pDevDesc. The wrapper
  // dispatches through GEcurrentDevice() which is valid during this call.
  //
  // Override this to report supported features. Call the base implementation
  // first if you only want to override specific entries.
  virtual SEXP dev_capabilities(SEXP cap)
  {
    auto c = cpp11::writable::list(cap);
    c[R_GE_capability_patterns] = cpp11::writable::integers({0});
    c[R_GE_capability_clippingPaths] = cpp11::writable::integers({0});
    c[R_GE_capability_masks] = cpp11::writable::integers({0});
    c[R_GE_capability_compositing] = cpp11::writable::integers({0});
    c[R_GE_capability_transformations] = cpp11::writable::integers({0});
    c[R_GE_capability_paths] = cpp11::writable::integers({0});
#if R_GE_version >= 16
    c[R_GE_capability_glyphs] = cpp11::writable::integers({0});
#endif
    return cpp11::as_sexp(c);
  }

  // =========================================================================
  //  FEATURE FLAGS
  //
  //  Set these *before* calling create(). They control which optional
  //  DevDesc callbacks are wired and what dev.capabilities() reports.
  // =========================================================================

  // Enable the dev_cap callback and report haveCapture=yes.
  bool m_df_cap = false;

  // Enable the R display list (allows recordPlot/replayPlot).
  bool m_df_displaylist = false;

  // Enable the dev_locator callback and report haveLocator=yes.
  bool m_df_locator = false;

  // =========================================================================
  //  INITIAL VALUES
  //
  //  Set these *before* calling create() to override defaults.
  //  Width/height/pointsize are passed through the constructor.
  // =========================================================================

  const double m_initial_width;
  const double m_initial_height;
  const double m_initial_pointsize;
  const int m_initial_fill = R_RGB(255, 255, 255);
  const int m_initial_col = R_RGB(0, 0, 0);

 private:
  // Allocate and populate a DevDesc, wiring all callbacks to this object's
  // virtual methods. Called once from create(). The DevDesc is calloc'd so
  // all fields start at zero/NULL; only non-zero values need explicit
  // assignment.
  pDevDesc setup(device_container* t_device_specific)
  {
    pDevDesc dd = (DevDesc*)calloc(1, sizeof(DevDesc));
    if (dd == nullptr)
    {
      return dd;
    }

    // Initial graphics state (sets par("fg"), par("bg"), etc.)
    dd->startfill = m_initial_fill;
    dd->startcol = m_initial_col;
    dd->startps = m_initial_pointsize;
    dd->startlty = 0;
    dd->startfont = 1;
    dd->startgamma = 1;
    dd->gamma = 1;

    // Drawing callbacks -- each lambda extracts the generic_dev from
    // dd->deviceSpecific and forwards to the corresponding virtual method.
    dd->activate = [](pDevDesc dd)
    {
      from_dd(dd)->dev_activate(dd);
    };
    dd->deactivate = [](pDevDesc dd)
    {
      from_dd(dd)->dev_deactivate(dd);
    };
    dd->close = [](pDevDesc dd)
    {
      auto* container = static_cast<device_container*>(dd->deviceSpecific);
      container->device->dev_close(dd);
      delete container;
    };
    dd->clip = [](double x0, double x1, double y0, double y1, pDevDesc dd)
    {
      from_dd(dd)->dev_clip(x0, x1, y0, y1, dd);
    };
    dd->size = [](double* left, double* right, double* bottom, double* top, pDevDesc dd)
    {
      *left = dd->left;
      *top = dd->top;
      *right = dd->right;
      *bottom = dd->bottom;

      from_dd(dd)->dev_size(left, right, bottom, top, dd);
    };
    dd->newPage = [](pGEcontext gc, pDevDesc dd)
    {
      from_dd(dd)->dev_newPage(gc, dd);
    };
    dd->line = [](double x1, double y1, double x2, double y2, pGEcontext gc, pDevDesc dd)
    {
      from_dd(dd)->dev_line(x1, y1, x2, y2, gc, dd);
    };
    dd->text = [](double x, double y, const char* str, double rot, double hadj,
                  pGEcontext gc, pDevDesc dd)
    {
      from_dd(dd)->dev_text(x, y, str, rot, hadj, gc, dd);
    };
    dd->strWidth = [](const char* str, pGEcontext gc, pDevDesc dd)
    {
      return from_dd(dd)->dev_strWidth(str, gc, dd);
    };
    dd->rect = [](double x0, double y0, double x1, double y1, pGEcontext gc, pDevDesc dd)
    {
      from_dd(dd)->dev_rect(x0, y0, x1, y1, gc, dd);
    };
    dd->circle = [](double x, double y, double r, pGEcontext gc, pDevDesc dd)
    {
      from_dd(dd)->dev_circle(x, y, r, gc, dd);
    };
    dd->polygon = [](int n, double* x, double* y, pGEcontext gc, pDevDesc dd)
    {
      from_dd(dd)->dev_polygon(n, x, y, gc, dd);
    };
    dd->polyline = [](int n, double* x, double* y, pGEcontext gc, pDevDesc dd)
    {
      from_dd(dd)->dev_polyline(n, x, y, gc, dd);
    };
    dd->path = [](double* x, double* y, int npoly, int* nper, Rboolean winding,
                  pGEcontext gc, pDevDesc dd)
    {
      from_dd(dd)->dev_path(x, y, npoly, nper, winding, gc, dd);
    };
    dd->mode = [](int mode, pDevDesc dd)
    {
      from_dd(dd)->dev_mode(mode, dd);
    };
    dd->metricInfo = [](int c, pGEcontext gc, double* ascent, double* descent,
                        double* width, pDevDesc dd)
    {
      from_dd(dd)->dev_metricInfo(c, gc, ascent, descent, width, dd);
    };
    dd->raster = [](unsigned int* raster, int w, int h, double x, double y, double width,
                    double height, double rot, Rboolean interpolate, pGEcontext gc,
                    pDevDesc dd)
    {
      from_dd(dd)->dev_raster(raster, w, h, x, y, width, height, rot, interpolate, gc,
                              dd);
    };
#if R_GE_version >= 13
    dd->setPattern = [](SEXP pattern, pDevDesc dd)
    {
      return from_dd(dd)->dev_setPattern(pattern, dd);
    };
    dd->releasePattern = [](SEXP ref, pDevDesc dd)
    {
      from_dd(dd)->dev_releasePattern(ref, dd);
    };
    dd->setClipPath = [](SEXP path, SEXP ref, pDevDesc dd)
    {
      return from_dd(dd)->dev_setClipPath(path, ref, dd);
    };
    dd->releaseClipPath = [](SEXP ref, pDevDesc dd)
    {
      from_dd(dd)->dev_releaseClipPath(ref, dd);
    };
    dd->setMask = [](SEXP path, SEXP ref, pDevDesc dd)
    {
      return from_dd(dd)->dev_setMask(path, ref, dd);
    };
    dd->releaseMask = [](SEXP ref, pDevDesc dd)
    {
      from_dd(dd)->dev_releaseMask(ref, dd);
    };
#endif
#if R_GE_version >= 15
    dd->defineGroup = [](SEXP source, int op, SEXP destination, pDevDesc dd)
    {
      return from_dd(dd)->dev_defineGroup(source, op, destination, dd);
    };
    dd->useGroup = [](SEXP ref, SEXP trans, pDevDesc dd)
    {
      return from_dd(dd)->dev_useGroup(ref, trans, dd);
    };
    dd->releaseGroup = [](SEXP ref, pDevDesc dd)
    {
      return from_dd(dd)->dev_releaseGroup(ref, dd);
    };
    dd->stroke = [](SEXP path, pGEcontext gc, pDevDesc dd)
    {
      return from_dd(dd)->dev_stroke(path, gc, dd);
    };
    dd->fill = [](SEXP path, int rule, pGEcontext gc, pDevDesc dd)
    {
      return from_dd(dd)->dev_fill(path, rule, gc, dd);
    };
    dd->fillStroke = [](SEXP path, int rule, pGEcontext gc, pDevDesc dd)
    {
      return from_dd(dd)->dev_fillStroke(path, rule, gc, dd);
    };

    // From "SEXP devcap(SEXP args)" in "grDevices/src/devices.c" it seems
    // that this function may be used to set some entries to the capability list.
    // Must return a list of the correct length and only integers.
    // ugd(); dev.capabilities()
    dd->capabilities = [](SEXP t_cap)
    {
      return from_dd(GEcurrentDevice()->dev)->dev_capabilities(t_cap);
    };
#endif
#if R_GE_version >= 16
    dd->glyph = [](int n, int* glyphs, double* x, double* y, SEXP font, double size,
                   int colour, double rot, pDevDesc dd)
    {
      from_dd(dd)->dev_glyph(n, glyphs, x, y, font, size, colour, rot, dd);
    };
#endif

    if (m_df_cap)
    {
      dd->cap = [](pDevDesc dd)
      {
        return from_dd(dd)->dev_cap(dd);
      };
    }
    else
    {
      dd->cap = nullptr;
    }

    // UTF-8 support: all text goes through the UTF-8 entry points
    // (textUTF8/strWidthUTF8), which we alias to the regular ones since
    // dev_text/dev_strWidth already handle UTF-8.
    dd->wantSymbolUTF8 = static_cast<Rboolean>(1);
    dd->hasTextUTF8 = static_cast<Rboolean>(1);
    dd->textUTF8 = dd->text;
    dd->strWidthUTF8 = dd->strWidth;
    dd->useRotatedTextInContour = static_cast<Rboolean>(1);

    // Screen Dimensions in pts
    dd->left = 0;
    dd->top = 0;
    dd->right = m_initial_width;
    dd->bottom = m_initial_height;

    // Initial clip region
    dd->clipLeft = dd->left;
    dd->clipTop = dd->top;
    dd->clipRight = dd->right;
    dd->clipBottom = dd->bottom;

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

    // Capabilities reported via dev.capabilities()
    // haveTransparency: 1=no, 2=yes
    // haveTransparentBg: 1=no, 2=fully, 3=semi-transparent
    // haveRaster: 1=no, 2=yes, 3=except missing values
    // haveCapture/haveLocator: 1=no, 2=yes
    dd->canClip = static_cast<Rboolean>(1);
    dd->canHAdj = 1;  // 0=none, 1={0,0.5,1}, 2=[0,1]
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
    dd->haveCapture = m_df_cap ? 2 : 1;
    dd->haveLocator = m_df_locator ? 2 : 1;

    dd->newFrameConfirm = [](pDevDesc dd)
    {
      return from_dd(dd)->dev_newFrameConfirm(dd);
    };
    dd->onExit = [](pDevDesc dd)
    {
      from_dd(dd)->dev_onExit(dd);
    };
    dd->eventEnv = R_NilValue;
    dd->eventHelper = [](pDevDesc dd, int code)
    {
      from_dd(dd)->dev_eventHelper(dd, code);
    };
    dd->holdflush = [](pDevDesc dd, int level)
    {
      return from_dd(dd)->dev_holdflush(dd, level);
    };
    if (m_df_locator)
    {
      dd->locator = [](double* x, double* y, pDevDesc dd)
      {
        return from_dd(dd)->dev_locator(x, y, dd);
      };
    }

#if R_GE_version >= 14
    // When TRUE, the engine skips its own clipping and relies entirely on
    // the device's dev_clip implementation.
    dd->deviceClip = static_cast<Rboolean>(0);
#endif

    // Advertise the highest graphics engine version this device supports.
    // The engine uses this to decide which callbacks are safe to call.
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
