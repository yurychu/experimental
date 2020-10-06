
#include <RunClient.hpp>

#include <iostream>
#include <string>
#include <cstdint>

#include <boost/asio.hpp>
namespace asio = boost::asio;


void run_client_1()
{
    std::cout << "Run client 1" << std::endl;

    std::string raw_ip_address = "127.0.0.1";
    uint16_t port_num = 3333;
    boost::system::error_code ec;
    asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, ec);
    if (ec.value() != 0){
        std::cout << "Failed to parse IP address. Err code = " << ec.value() << ". Message: " << ec.message() << std::endl;
        return;
    }
    asio::ip::tcp::endpoint ep { ip_address, port_num };
}


void run_client_2()
{
    asio::io_service ios{};
    asio::ip::tcp protocol = asio::ip::tcp::v4();
    asio::ip::tcp::socket sock { ios };
    boost::system::error_code ec;
    sock.open(protocol, ec);
    if (ec.value() != 0){
        std::cout << "Failed to open socket. Err code: " << ec.value() << ". Message: " << ec.message() << std::endl;
        return;
    }

}

void run_client_3()
{
    std::string host = "example.com";
    std::string port = "3333";

    asio::io_service ios{};

    asio::ip::tcp::resolver::query res_query {host, port, asio::ip::tcp::resolver::query::numeric_service};

    asio::ip::tcp::resolver rslvr { ios };

    boost::system::error_code ec;

    asio::ip::tcp::resolver::iterator it = rslvr.resolve(res_query, ec);
    if (ec.value() != 0){
        std::cout << "Failed to resolve DNS name: " << ec.value() << ", Msg: " << ec.message() << std::endl;
    }
}
