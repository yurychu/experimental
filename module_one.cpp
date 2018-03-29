#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

#include "protocol.hpp"
#include "FunctionalServer.hpp"


namespace asio = boost::asio;
namespace cmt = communication;

using ipc_protocol = boost::asio::local::stream_protocol;


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


class Module
{
private:
    std::string itsName;

public:
    explicit Module(std::string name)
            : itsName(std::move(name))
    {
    };

    ~Module() = default;

    void start(){
        asio::io_service itsIos;

        auto client = std::make_shared<Client>(itsIos);
        auto func_part = std::make_shared<cmt::FunctionalPart>();
        auto server = std::make_shared<cmt::Server>(itsIos, itsName, func_part);

        server->init_to_run();

        itsIos.run();
    }

};


int main()
{
    Module module("module_one");

    module.start();

    return 0;
}