#include <unigd_api/client.h>
#include "unigd_dev.h"
#include "renderers.h"

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
}

[[cpp11::external]]
int ugd_c_test_fun()
{
    return 7;
}

[[cpp11::external("ugd_attach_client", "Attach client to unigd device.")]]
bool ugd_c_attach_client(int devnum, const std::shared_ptr<unigd::graphics_client>& t_client)
{
    auto dev = validate_unigddev(devnum);
    dev->attach_client(t_client);

    return true;
}

[[cpp11::external("ugd_renderers_find")]]
bool ugd_c_renderers_find(const std::__cxx11::string& id, const unigd::renderers::renderer_gen** renderer)
{
    return unigd::renderers::find_renderer(id, renderer);
}

[[cpp11::external("ugd_renderers_find_info")]]
bool ugd_c_renderers_find_info(const std::__cxx11::string& id, const unigd::renderers::renderer_info** renderer)
{
    return unigd::renderers::find_info(id, renderer);
}