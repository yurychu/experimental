#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>


namespace asio = boost::asio;


using ipc_protocol = boost::asio::local::stream_protocol;


class Session
        : public std::enable_shared_from_this<Session>
{
private:
    std::shared_ptr<ipc_protocol::socket> itsSocket;

    asio::streambuf itsRequest;

public:
    explicit Session(std::shared_ptr<ipc_protocol::socket> socket)
            : itsSocket(socket)
    {

    }

    void start_communicate()
    {
        auto self(shared_from_this());
        asio::async_read_until(*itsSocket, itsRequest, '\n',
                               [this, self]
                                       (const boost::system::error_code & ec,
                                        std::size_t bytes_transferred)
                               {
                                   this->on_request_received(ec, bytes_transferred);
                               });
    }

private:
    void on_request_received(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {
        if (!ec.value()){
            auto self(shared_from_this());
            std::cout << std::istream(&itsRequest).rdbuf() << std::endl;

            asio::async_write(*itsSocket, asio::buffer("-- Was taken --\n"),
                              [this, self](const boost::system::error_code & ec,
                                     std::size_t bytes_transferred)
                              {
                                  this->on_response_sent(ec, bytes_transferred);
                              }
            );

        }
        else {
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
        }
    }

    void on_response_sent(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {
        if (ec.value() != 0){
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
        }
        else {
            auto self(shared_from_this());
            asio::async_read_until(*itsSocket, itsRequest, '\n',
                                   [this, self]
                                           (const boost::system::error_code &ec,
                                            std::size_t bytes_transferred) {
                                       this->on_request_received(ec, bytes_transferred);
                                   });
        }
    }

};


class Acceptor
{
private:
    asio::io_service & itsIos;
    ipc_protocol::acceptor itsAcceptor;

public:
    Acceptor(asio::io_service & ios, std::string acc_name)
    : itsIos(ios),
      itsAcceptor(ios)
    {
        if(acc_name.find('.') == std::string::npos){
            acc_name += ".socket";
        }
        ::unlink(acc_name.c_str());
        ipc_protocol::endpoint ep(acc_name);

        itsAcceptor.open(ep.protocol());
        itsAcceptor.bind(ep);
    }

    void start()
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
            auto session = std::make_shared<Session>(sock);
            session->start_communicate();

            // may be check if cancelling
            this->init_accept();
        }
        else {
            std::cout<< "Error occured! Error code = "
                     << ec.value()
                     << ". Message: " <<ec.message();
        }



    }

};


class Server
{
private:
    asio::io_service & itsIos;
    Acceptor itsAcceptor;

public:
    explicit Server(asio::io_service & ios, const std::string & ep_name)
            : itsIos(ios),
              itsAcceptor(ios, ep_name)
    {
        itsAcceptor.start();
    }

    ~Server() = default;

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

class Module
{
private:
    asio::io_service itsIos;
    Server itsServer;
    Client itsClient;

public:
    explicit Module(const std::string & name)
    : itsServer(itsIos, name),
      itsClient(itsIos)
    {

    };

    ~Module() = default;

    void start_ios(){
        itsIos.run();
    }

    void stop_ios(){
        itsIos.stop();
    }
};


int main()
{
    Module module("module_one");

    module.start_ios();

    return 0;
}