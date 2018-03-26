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
            std::cout << std::istream(&itsRequest).rdbuf() << std::endl;
        }
        else {
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
        }

    }
};


class Acceptor
{
private:
    asio::io_service & itsIos;
    ipc_protocol::acceptor itsAcceptor;

public:
    Acceptor(asio::io_service & ios, const std::string & acc_name)
    : itsIos(ios),
      itsAcceptor(ios)
    {
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
        }
        else {
            std::cout<< "Error occured! Error code = "
                     << ec.value()
                     << ". Message: " <<ec.message();
        }

        this->init_accept();

    }

};


class Server
{
private:
    asio::io_service & itsIos;
    Acceptor itsAcceptor;

public:
    explicit Server(asio::io_service & ios)
            : itsIos(ios),
              itsAcceptor(ios, "module_one.socket")
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
    Module()
    : itsServer(itsIos),
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
    Module module;

    module.start_ios();

    std::cout << "Start sleeping" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(30));

    module.stop_ios();

}