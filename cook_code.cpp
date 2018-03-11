#include <cstdint>
#include <iostream>

#include <boost/asio.hpp>


using namespace boost;

int main(){

    // binding to endpoint, all available
    u_short port_num = 3333;
    asio::ip::tcp::endpoint ep(
            asio::ip::address_v4::any(),
            port_num
    );

    asio::io_service ios;
    asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

    return 0;
}
