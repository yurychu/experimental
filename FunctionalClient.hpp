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
        std::string itsDestinationName;
        std::shared_ptr<ipc_protocol::socket> itsSocket;

    public:
        ClientSession(std::string destination_name,
                      std::shared_ptr<ipc_protocol::socket> socket)
                : itsDestinationName(destination_name),
                  itsSocket(socket)
        {

            if(destination_name.find('.') == std::string::npos){
                destination_name += ".socket";
            }
            std::cout << "Create new sesstion with: " << destination_name << std::endl;

            ipc_protocol::endpoint ep("./sockets/" + destination_name);
            socket->async_connect(ep,
                                  [this]
                                          (const boost::system::error_code & ec)
            {
                this->on_connected(ec);
            });
        }

        ~ClientSession() = default;

        void make_request(const prtcl::Body & body)
        {

        }

    private:
        void on_connected(const boost::system::error_code & ec)
        {
            std::cout << "on_connected: " << std::endl;
            if (!ec.value()){
                std::cout << "Connected without errors." << std::endl;
            }
            else {
                std::cout<< "Error occured! Error code = "
                         << ec.value()
                         << ". Message: " << ec.message() << std::endl;
            }
        }
    };

    class Client
    {
    private:
        asio::io_service & itsIos;
        std::map<std::string, std::shared_ptr<ClientSession>> itsSessions;


    public:
        explicit Client(asio::io_service & ios)
                : itsIos(ios)
        {
        }

        ~Client() = default;

        void send_data_to(const std::string & destination, const prtcl::Body & body)
        {
            auto it = itsSessions.find(destination);
            if (it != itsSessions.end()){
                it->second->make_request(body);
            }
            else {
                auto sock = std::make_shared<ipc_protocol::socket>(itsIos);
                itsSessions[destination] = std::make_shared<ClientSession>(destination, sock);
            }

        }

    };
}


#endif //EXPERIMENTAL_SYSTEM_FUNCTIONALCLIENT_HPP
