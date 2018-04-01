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

        prtcl::Header itsHeader;
        prtcl::Body itsBody;

    public:
        ClientSession(std::string destination_name,
                      std::shared_ptr<ipc_protocol::socket> socket)
                : itsDestinationName(destination_name),
                  itsSocket(socket)
        {

            if(destination_name.find('.') == std::string::npos){
                destination_name += ".socket";
            }
            std::cout << "Create new session with: " << destination_name << std::endl;

            ipc_protocol::endpoint ep("./sockets/" + destination_name);
            socket->async_connect(ep,
                                  [this]
                                          (const boost::system::error_code & ec)
            {
                this->on_connected(ec);
            });
        }

        ~ClientSession() = default;

        void make_request(const std::string & content)
        {
            itsBody.set_as_empty();
            itsBody.add_content(content);
            itsHeader.set_body_length(itsBody.get_length());

            asio::async_write(*itsSocket,
                              asio::buffer(itsHeader.get_buffer()),
                              [this]
                                      (const boost::system::error_code & ec, size_t bytes_transferred)
                              {
                                  this->on_header_written(ec, bytes_transferred);
                              }
            );

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

        void on_header_written(const boost::system::error_code & ec, size_t bytes_transferred)
        {
            std::cout << "on_header_written: " << std::endl;
            if (!ec.value()){
                std::cout << "Bytes transferred: " << bytes_transferred << std::endl;

                asio::async_write(*itsSocket,
                                  asio::buffer(itsBody.get_buffer(), itsBody.get_length()),
                                  [this]
                                          (const boost::system::error_code & ec, size_t bytes_transferred)
                                  {
                                      this->on_body_written(ec, bytes_transferred);
                                  }
                );
            }
            else {
                std::cout<< "Error occured! Error code = "
                         << ec.value()
                         << ". Message: " << ec.message() << std::endl;
            }
        }

        void on_body_written(const boost::system::error_code & ec, size_t bytes_transferred)
        {
            std::cout << "on_body_written: " << std::endl;
            if (!ec.value()){
                std::cout << "Bytes transferred: " << bytes_transferred << std::endl;

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

        void send_data_to(const std::string& destination, const std::string& content)
        {
            auto it = itsSessions.find(destination);
            if (it != itsSessions.end()){
                it->second->make_request(content);
            }
            else {
                auto sock = std::make_shared<ipc_protocol::socket>(itsIos);
                itsSessions[destination] = std::make_shared<ClientSession>(destination, sock);
                itsSessions[destination]->make_request(content);
            }

        }

    };
}


#endif //EXPERIMENTAL_SYSTEM_FUNCTIONALCLIENT_HPP
