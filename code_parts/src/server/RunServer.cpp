
#include <RunServer.hpp>

#include <iostream>
#include <string>
#include <cstdint>

#include <boost/asio.hpp>
namespace asio = boost::asio;


void run_server_1()
{
    uint16_t port_num = 3333;

    asio::ip::address ip_addr = asio::ip::address_v6::any();

    asio::ip::tcp::endpoint ep { ip_addr, port_num };


}

void run_server_2()
{
    asio::io_service ios;
    asio::ip::tcp protocol = asio::ip::tcp::v6();
    asio::ip::tcp::acceptor acc { ios };
    boost::system::error_code ec;
    acc.open(protocol, ec);
    if (ec.value() != 0){
        std::cout << "Failed to open socket: " << ec.value() << "Msg: " << ec.message() << std::endl;
    }

}


void run_server_3()
{
    uint16_t port_num = 3333;
    asio::ip::address ip_addr = asio::ip::address_v4::any();

    asio::ip::tcp::endpoint ep { ip_addr, port_num };
    asio::ip::tcp protocol = ep.protocol(); // v4 from ip_addr

    asio::io_service ios;
    asio::ip::tcp::acceptor acc { ios };
    boost::system::error_code ec;
    acc.open(protocol, ec);
    if (ec.value() != 0){
        std::cout << "Fail open acceptor, err code: " << ec.value() << ", msg: " << ec.message() << std::endl;
        return;
    }

    acc.bind(ep, ec);
    if (ec.value() != 0){
        std::cout << "Fail bind acc to ep, code: " << ec.value() << ", msg: " << ec.message() << std::endl;
        return;
    }

    std::cout << "ep.addr() to string: " << ep.address().to_string() << std::endl;

}
