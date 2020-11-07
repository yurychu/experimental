
#include <RunServer.hpp>

#include <thread>
#include <iostream>
#include <string>
#include <cstdint>

#include <boost/asio.hpp>
namespace asio = boost::asio;


void run_server_1()
{
    // protocol for listen incoming
    uint16_t port_num = 3333;

    // all addresses
    // need version v4 or v6
    asio::ip::address ip_addr = asio::ip::address_v6::any();

    // endpoint represents all addresses on host
    asio::ip::tcp::endpoint ep { ip_addr, port_num };

}

void run_server_2()
{
    asio::io_service ios;
    asio::ip::tcp protocol = asio::ip::tcp::v6();
    asio::ip::tcp::acceptor acc { ios }; // passive socket
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

    acc.bind(ep, ec);  // OS still reject connections requests. Need to listen mode switch.
    if (ec.value() != 0){
        std::cout << "Fail bind acc to ep, code: " << ec.value() << ", msg: " << ec.message() << std::endl;
        return;
    }

    const int BACKLOG_SIZE = 20;
    acc.listen(BACKLOG_SIZE); // switch acc socket to listen mode, OS not rejects requests here.

    // new active socket for communicate concrete clients
    asio::ip::tcp::socket sock { ios };
    acc.accept(sock); // accept request to this socket from requests queue.
    // accept is blocking call of executing thread

    std::cout << "To sleep for ..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds{99});


    std::cout << "End." << std::endl;
}
