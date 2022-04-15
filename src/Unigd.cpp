#include <cpp11/function.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/list.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/logicals.hpp>
#include <cpp11/data_frame.hpp>
#include <cpp11/as.hpp>
#include <cpp11/raws.hpp>

#include <vector>
#include <string>

#include "unigd_dev.h"
#include "uuid.h"
#include "unigd_version.h"
#include "renderer_svg.h"
#include "renderers.h"
#include "r_thread.h"


namespace
{
    inline unigd::HttpgdDev *getDev(pDevDesc dd)
    {
        return static_cast<unigd::HttpgdDev *>(dd->deviceSpecific);
    }

    inline unigd::HttpgdDev *validate_unigddev(int devnum)
    {
        if (devnum < 1 || devnum > 64) // R_MaxDevices
        {
            cpp11::stop("invalid graphical device number");
        }

        pGEDevDesc gdd = GEgetDevice(devnum - 1);
        if (!gdd)
        {
            cpp11::stop("invalid device");
        }
        pDevDesc dd = gdd->dev;
        if (!dd)
        {
            cpp11::stop("invalid device");
        }
        auto dev = static_cast<unigd::HttpgdDev *>(dd->deviceSpecific);
        if (!dev)
        {
            cpp11::stop("invalid device");
        }

        return dev;
    }

    inline long validate_plotid(const std::string &id)
    {
        try {
            return std::stol(id);
        }
        catch (const std::exception &e){
            cpp11::stop("Not a valid plot ID.");
        }
        return -1;
    }

} // namespace unigd


[[cpp11::register]]
int unigd_ugd_(std::string bg, double width, double height,
             double pointsize, cpp11::list aliases,
             bool reset_par)
{
    int ibg = R_GE_str2col(bg.c_str());

    auto dev = new unigd::HttpgdDev(
        {ibg,
         width,
         height,
         pointsize,
         aliases,
         reset_par});

    return unigd::HttpgdDev::make_device("unigd", dev);
}

[[cpp11::register]]
cpp11::list unigd_state_(int devnum)
{
    auto dev = validate_unigddev(devnum);

    unigd::device_state state = dev->api_state();

    using namespace cpp11::literals;
    return cpp11::writable::list{
        "hsize"_nm = state.hsize,
        "upid"_nm = state.upid,
        "active"_nm = state.active};
}

[[cpp11::register]]
cpp11::list unigd_info_(int devnum)
{
    /*auto dev = validate_unigddev(devnum);*/

    using namespace cpp11::literals;
    return cpp11::writable::list{
        "version"_nm = cpp11::writable::list{
        "unigd"_nm = UNIGD_VERSION,
        "cairo"_nm = UNIGD_VERSION_CAIRO
        }
    };
}

[[cpp11::register]]
cpp11::data_frame unigd_renderers_()
{
    using namespace cpp11::literals;

    const auto renderers = unigd::renderers::renderers();

    cpp11::writable::list rens{static_cast<R_xlen_t>(renderers->size())};

    const R_xlen_t nren = renderers->size();
    cpp11::writable::strings ren_id{nren};
    cpp11::writable::strings ren_mime{nren};
    cpp11::writable::strings ren_ext{nren};
    cpp11::writable::strings ren_name{nren};
    cpp11::writable::strings ren_type{nren};
    cpp11::writable::logicals ren_text;
    ren_text.resize(nren); // R cpp11 bug?
    cpp11::writable::strings ren_descr{nren};

    R_xlen_t i = 0;
    for (auto it = renderers->begin(); it != renderers->end(); it++)
    {
        ren_id[i] = it->second.info.id;
        ren_mime[i] = it->second.info.mime;
        ren_ext[i] = it->second.info.fileext;
        ren_name[i] = it->second.info.name;
        ren_type[i] = it->second.info.type;
        ren_text[i] = it->second.info.text;
        ren_descr[i] = it->second.info.description;
        i++;
    }

    cpp11::writable::data_frame res({
                "id"_nm = ren_id,
                "mime"_nm = ren_mime,
                "ext"_nm = ren_ext,
                "name"_nm = ren_name,
                "type"_nm = ren_type,
                "text"_nm = ren_text,
                "descr"_nm = ren_descr
    });
    return res;
}


[[cpp11::register]]
int unigd_plot_find_(int devnum, std::string plot_id)
{
    long pid = validate_plotid(plot_id);
    auto dev = validate_unigddev(devnum);
    auto page = dev->api_index(pid);
    if (page == -1)
    {
        cpp11::stop("Not a valid plot ID.");
    }
    return page;
}

[[cpp11::register]]
SEXP unigd_render_(int devnum, int page, double width, double height, double zoom, std::string renderer_id)
{
    auto dev = validate_unigddev(devnum);

    if (width < 0 || height < 0)
    {
        zoom = 1;
    }

    const unigd::renderers::renderer_gen *ren;
    auto fi_renderer = unigd::renderers::find_renderer(renderer_id, &ren);
    if (!fi_renderer)
    {
        cpp11::stop("Not a valid string renderer ID.");
    }
    auto renderer = ren->renderer();
    dev->api_render(page, width / zoom, height / zoom, renderer.get(), zoom);

    const uint8_t *buf;
    size_t buf_size;
    renderer->get_data(&buf, &buf_size);

    if (ren->info.text) {
        return cpp11::writable::strings({ cpp11::r_string(std::string(buf, buf+buf_size)) });
    } else {
        return cpp11::writable::raws(buf, buf+buf_size);
    }
}

[[cpp11::register]]
bool unigd_remove_(int devnum, int page)
{
    auto dev = validate_unigddev(devnum);
    return dev->api_remove(page);
}

[[cpp11::register]]
bool unigd_remove_id_(int devnum, std::string id)
{
    long pid = validate_plotid(id);
    auto dev = validate_unigddev(devnum);
    auto page = dev->api_index(pid);
    if (page == -1)
    {
        cpp11::stop("Not a valid plot ID.");
    }

    return dev->api_remove(page);
}

[[cpp11::register]]
cpp11::writable::list unigd_id_(int devnum, int page, int limit)
{
    auto dev = validate_unigddev(devnum);
    unigd::device_api_query_result res;

    if (page == -1)
    {
        res = dev->api_query_index(page);
    }
    else
    {
        res = dev->api_query_range(page, limit);
    }

    using namespace cpp11::literals;
    cpp11::writable::list state{
        "hsize"_nm = res.state.hsize,
        "upid"_nm = res.state.upid,
        "active"_nm = res.state.active};

    cpp11::writable::list plots{static_cast<R_xlen_t>(res.ids.size())};

    for (std::size_t i = 0; i < res.ids.size(); ++i)
    {
        cpp11::writable::list p{"id"_nm = std::to_string(res.ids[i])};
        p.attr("class") = "unigd_pid";
        plots[i] = p;
    }

    return {
        "state"_nm = state,
        "plots"_nm = plots
    };
}

[[cpp11::register]]
bool unigd_clear_(int devnum)
{
    auto dev = validate_unigddev(devnum);
    return dev->api_clear();
}

[[cpp11::register]]
void unigd_ipc_open_()
{
    unigd::async::ipc_open();
}

[[cpp11::register]]
void unigd_ipc_close_()
{
    unigd::async::ipc_close();
}
