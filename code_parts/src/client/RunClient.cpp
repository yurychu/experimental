
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
    // very useful, validate addresses
    asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, ec);
    if (ec.value() != 0){
        std::cout << "Failed to parse IP address. Err code = " << ec.value() << ". Message: " << ec.message() << std::endl;
        return;
    }
    // endpoint is ip address and port.
    // designate server application here
    asio::ip::tcp::endpoint ep { ip_address, port_num };
}


void run_client_2()
{
    asio::io_service ios{};
    asio::ip::tcp::socket sock { ios }; // just create io operations
    // from protocol tcp, no real in Operating System

    asio::ip::tcp protocol = asio::ip::tcp::v4(); // protocol object
    // data structure with settings

    boost::system::error_code ec;
    sock.open(protocol, ec); // open socket with protocol object, real create in Operation System
    if (ec.value() != 0){
        std::cout << "Failed to open socket. Err code: " << ec.value() << ". Message: " << ec.message() << std::endl;
        return;
    }

}

void run_client_3()
{
    std::string host = "vk.com";
    std::string port = "80"; // string, resolver::query parse it.

    asio::io_service ios{};

    // query to dns
    // port number for construct end points.
    // numeric service - port as number, not specific protocol.
    asio::ip::tcp::resolver::query res_query {host, port, asio::ip::tcp::resolver::query::numeric_service};

    asio::ip::tcp::resolver rslvr { ios };  // use io operations

    boost::system::error_code ec;

    asio::ip::tcp::resolver::iterator it = rslvr.resolve(res_query, ec);
    if (ec.value() != 0){
        std::cout << "Failed to resolve DNS name: " << ec.value() << ", Msg: " << ec.message() << std::endl;
        return;
    }
    asio::ip::tcp::resolver::iterator it_end; // end iterator
    for (; it != it_end; ++it){
        asio::ip::tcp::endpoint ep = it->endpoint();
        std::cout << ep.address().to_string() << std::endl;
    }

}

void run_client_4()
{
    std::string raw_ip_addr = "127.0.0.1";
    uint16_t port_num = 3333;
    boost::system::error_code ec;

    asio::ip::address ip_addr = asio::ip::address::from_string(raw_ip_addr, ec);
    if (ec){
        std::cout << "Fail create ip addr from string: " << ec.value() << ", msg: " << ec.message() << std::endl;
        return;
    }
    asio::ip::tcp::endpoint ep { ip_addr, port_num };
    asio::ip::tcp protocol = ep.protocol();
    asio::io_service ios{};

    asio::ip::tcp::socket sock { ios };
    sock.open(protocol, ec);
    if (ec){
        std::cout << "Fail to open sock, code: " << ec.value() << ", msg: " << ec.message() << std::endl;
        return;
    }

    // we dont bind, but sock binded in connect().

    sock.connect(ep, ec); // blocking running thread call
    if (ec){
        std::cout << "Fail connect to ep, code: " << ec.value() << ", msg: " << ec.message() << std::endl;
        return;
    }
    // has also asio::connect(sock, it); iterate over resolved eps and try connect, no need open() before call, v4 or v6

}
