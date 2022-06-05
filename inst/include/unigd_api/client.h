#ifndef UNIGD_EXTERNAL_CLIENT_H
#define UNIGD_EXTERNAL_CLIENT_H

#include <memory>
#include <unigd_api/device.h>

namespace unigd
{
    class graphics_client
    {
    public:
        virtual void start() { };
        virtual void close() { };
        virtual void broadcast_state_current() {};
        virtual int client_id() { return 0; };
        virtual std::string client_status() { return ""; };

        std::weak_ptr<device_api> api;
    };

} // namespace unigd


#endif // UNIGD_EXTERNAL_CLIENT_H