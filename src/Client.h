#ifndef CLIENT_H
#define CLIENT_H


namespace unigd
{
    class Client
    {
    public:
        virtual void start() { };
        virtual void stop() { };
        virtual void broadcast_state_current() {};
    };

} // namespace unigd


#endif // CLIENT_H