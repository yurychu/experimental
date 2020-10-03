#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

#include "protocol.hpp"
#include "FunctionalServer.hpp"
#include "FunctionalClient.hpp"

namespace cmt = communication;


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

        auto client = std::make_shared<cmt::Client>(itsIos);
        auto func_part = std::make_shared<cmt::FunctionalPart>(client);
        auto server = std::make_shared<cmt::Server>(itsIos, itsName, func_part);

        func_part->run_threads();
//        func_part->push("say_hello");  // create task for doing
        server->init_to_run();

        itsIos.run();
    }

};


int main()
{
    Module module("module_sender");
    module.start();

    return 0;
}
