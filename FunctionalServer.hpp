#ifndef EXPERIMENTAL_SYSTEM_FUNCTIONALSERVER_HPP
#define EXPERIMENTAL_SYSTEM_FUNCTIONALSERVER_HPP

#include <string>
#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

#include "protocol.hpp"
#include "TaskFunctional.hpp"


namespace asio = boost::asio;


using ipc_protocol = boost::asio::local::stream_protocol;


namespace communication
{
    class DefaultFunctionalPart
    {
    public:
        void push(std::string param1, std::string param2)
        {
            std::cout << "Puhsed to functional part: " << param1 << " " << param2 << std::endl;
        }
    };


#ifdef CUSTOM_FUNCTIONAL_PART
    using FunctionalPart = CustomFunctionalPart;
#else
    using FunctionalPart = DefaultFunctionalPart;
#endif


    class ServerSession
            : public std::enable_shared_from_this<ServerSession>
    {
    private:
        std::shared_ptr<FunctionalPart> itsFunctionalPart;
        std::shared_ptr<ipc_protocol::socket> itsSocket;

        prtcl::Header itsHeader;
        prtcl::Body itsBody;

    public:
        explicit ServerSession(std::shared_ptr<ipc_protocol::socket> socket, std::shared_ptr<FunctionalPart> func_part)
                : itsFunctionalPart(std::move(func_part)),
                  itsSocket(std::move(socket)),
                  itsHeader(),
                  itsBody()
        {
            std::cout << "Created new session" << std::endl;
        }

        void start_communicate()
        {
            this->read_header();
        }

    private:

        void read_header()
        {
            auto self(shared_from_this());
            asio::async_read(*itsSocket, asio::buffer(itsHeader.get_buffer()),
                             [this, self]
                                     (const boost::system::error_code & ec,
                                      std::size_t bytes_transferred)
                             {
                                 this->on_read_header(ec, bytes_transferred);
                             });
        }

        void on_read_header(const boost::system::error_code & ec,
                            std::size_t bytes_transferred
        ){
            if (!ec.value()) {
                std::cout << "Header read bytes transferred: " << bytes_transferred << std::endl
                          << "Metadata size: " << itsHeader.get_body_length() << std::endl;

                itsBody.set_length(itsHeader.get_body_length());

                auto self(shared_from_this());
                asio::async_read(*itsSocket, asio::buffer(itsBody.get_buffer(), itsBody.get_length()),
                                 [this, self]
                                         (const boost::system::error_code & ec,
                                          std::size_t bytes_transferred)
                                 {
                                     this->on_read_body(ec, bytes_transferred);
                                 });
            }
            else {
                std::cout << "Error occured! Error code = "
                          << ec.value()
                          << ". Message: " << ec.message();
            }
        }

        void on_read_body(const boost::system::error_code & ec,
                          std::size_t bytes_transferred){
            if (!ec.value()) {
                std::cout << "Body read bytes transferred: " << bytes_transferred << std::endl
                          << "Data: " << itsBody.get_data() << std::endl;

                // make functionality
                itsFunctionalPart->push(itsBody.get_data());

                this->read_header();
            }
            else {
                std::cout << "Error occured! Error code = "
                          << ec.value()
                          << ". Message: " << ec.message();
            }
        }

    };


    class Server
    {
    private:
        asio::io_service & itsIos;
        ipc_protocol::acceptor itsAcceptor;

        std::shared_ptr<FunctionalPart> itsFuncPart;

    public:
        Server(asio::io_service & ios, std::string acc_name, std::shared_ptr<FunctionalPart> func_part)
                : itsIos(ios),
                  itsAcceptor(ios),
                  itsFuncPart(std::move(func_part))
        {
            if(acc_name.find('.') == std::string::npos){
                acc_name += ".socket";
            }
            acc_name = "./sockets/" + acc_name;
            ::unlink(acc_name.c_str());
            ipc_protocol::endpoint ep(acc_name);

            itsAcceptor.open(ep.protocol());
            itsAcceptor.bind(ep);
        }

        void init_to_run()
        {
            itsAcceptor.listen();
            this->init_accept();
        }

    private:
        void init_accept()
        {
            auto sock = std::make_shared<ipc_protocol::socket>(itsIos);
            itsAcceptor.async_accept(*sock,
                                     [this, sock]
                                             (const boost::system::error_code & ec)
                                     {
                                         this->on_accept(ec, sock);
                                     });
        }

        void on_accept(const boost::system::error_code & ec, std::shared_ptr<ipc_protocol::socket> sock)
        {
            std::cout << "on_accept" << std::endl;
            if (!ec.value()){
                auto session = std::make_shared<ServerSession>(sock, itsFuncPart);
                session->start_communicate();

                // may be check if cancelling
                this->init_accept();
            }
            else {
                std::cout<< "Error occured! Error code = "
                         << ec.value()
                         << ". Message: " << ec.message();
            }

        }

    };

}


#endif //EXPERIMENTAL_SYSTEM_FUNCTIONALSERVER_HPP
