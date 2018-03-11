#include <cstdint>
#include <iostream>

#include <boost/asio.hpp>


using namespace boost;

int main1(){

    std::string raw_ip_address = "127.0.0.1";
    u_short port_num = 8002;

    boost::system::error_code ec;

    asio::ip::address ip_address =
            asio::ip::address::from_string(raw_ip_address, ec);

    if (ec.value() != 0){
        std::cout << "Failed to parse the IP address. Error code = "
                  << ec.value() << ". Message: " << ec.message();
        return ec.value();
    }

    asio::ip::tcp::endpoint ep(ip_address, port_num);

    return 0;
}


int main2(){

    u_short port_num = 8002;

    boost::system::error_code ec;

    asio::ip::address ip_address =
            asio::ip::address_v6::any();

    asio::ip::tcp::endpoint ep(ip_address, port_num);

    return 0;
}


int main3(){

    // active socket example

    asio::io_service ios;

    asio::ip::udp protocol = asio::ip::udp::v4();

//    asio::ip::tcp::socket sock(ios);  // not allocated real in OS
//
//    boost::system::error_code ec;
//
//    sock.open(protocol, ec);  // allocated in OS for real communicate
//
//    if (ec.value() != 0){
//        std::cout << "Failed to open socket."
//                  << ec.value()
//                  << ". Message: " << ec.message();
//        return ec.value();
//    }
    // or
    try {
        asio::ip::udp::socket sock(ios, protocol);

    } catch (boost::system::system_error & e){
        std::cout << "Error: code = "
                  << e.code()
                  << ". Message: "
                  << e.what();
    }

    return 0;
}

int main4(){

    // passive socket via acceptor example

    asio::io_service ios;
    asio::ip::tcp protocol = asio::ip::tcp::v6();

    asio::ip::tcp::acceptor acceptor(ios);

    boost::system::error_code ec;

    acceptor.open(protocol, ec);

    if (ec.value() != 0) {
        std::cout
                << "Failed to open the acceptor socket! "
                << "Error code = " << ec.value()
                << ". Message: " << ec.message();

        return ec.value();
    }

    return 0;
}


int main5(){

    // DNS resolving

    std::string host = "google.com";
    std::string port_num = "3333";

    asio::io_service ios;

    asio::ip::tcp::resolver::query resolver_query(
            host,
            port_num,
            asio::ip::tcp::resolver::query::numeric_service
    );

    asio::ip::tcp::resolver resolver(ios);

    boost::system::error_code ec;

    asio::ip::tcp::resolver::iterator it =
            resolver.resolve(resolver_query, ec);

    if (ec.value() != 0){
        std::cout << "Failed to resolve a DNS name."
                  << "Error code = " << ec.value()
                  << ". Message: " << ec.message();
        return ec.value();
    }

    asio::ip::tcp::resolver::iterator it_end;
    for (; it != it_end; ++it){
        asio::ip::tcp::endpoint ep = it->endpoint();
        std::cout << ep.address() << std::endl;
    }

    return 0;
}