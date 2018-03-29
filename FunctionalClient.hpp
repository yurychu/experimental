#ifndef EXPERIMENTAL_SYSTEM_FUNCTIONALCLIENT_HPP
#define EXPERIMENTAL_SYSTEM_FUNCTIONALCLIENT_HPP

#include <string>
#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

#include "protocol.hpp"


namespace asio = boost::asio;


using ipc_protocol = boost::asio::local::stream_protocol;


namespace communication
{
    class ClientSession
    {
    private:


    public:
        ClientSession();
        ~ClientSession() = default;
    };

    class Client
    {
    private:
        asio::io_service & itsIos;

    public:
        explicit Client(asio::io_service & ios)
                : itsIos(ios)
        {
        }

        ~Client() = default;
    };
}


#endif //EXPERIMENTAL_SYSTEM_FUNCTIONALCLIENT_HPP
