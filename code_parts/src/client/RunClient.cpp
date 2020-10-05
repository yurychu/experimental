
#include <RunClient.hpp>

#include <iostream>
#include <string>
#include <cstdint>

#include <boost/asio.hpp>

namespace asio = boost::asio;

void run_client_1()
{
    std::cout << "Run client 1" << std::endl;

    std::string raw_ip_address = "1273.0.0.1";
    uint16_t port_num = 3333;

    boost::system::error_code ec;

    asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, ec);
    if (ec.value() != 0){
        std::cout << "Failed to parse IP address. Err code = " << ec.value() << ". Message: " << ec.message() << std::endl;
        return;
    }

    asio::ip::tcp::endpoint ep(ip_address, port_num);

}
