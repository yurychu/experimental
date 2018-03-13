#include <cstdint>
#include <iostream>
#include <memory>

#include <boost/asio.hpp>


using namespace boost;


std::string read_from_socket(asio::ip::tcp::socket & sock)
{
    const unsigned char MESSAGE_SIZE = 7;
    char buf[MESSAGE_SIZE];
    std::size_t total_bytes_read = 0;

    while (total_bytes_read != MESSAGE_SIZE){
        total_bytes_read += sock.read_some(
                asio::buffer(buf + total_bytes_read,
                             MESSAGE_SIZE - total_bytes_read)
        );
    }

    return std::string(buf, total_bytes_read);
}


std::string read_from_socket_read(asio::ip::tcp::socket & sock){
    const unsigned char MESSAGE_SIZE = 7;
    char buf[MESSAGE_SIZE];
    asio::read(sock, asio::buffer(buf, MESSAGE_SIZE));
    return std::string(buf, MESSAGE_SIZE);
}

std::string read_from_socket_delim_read_until(asio::ip::tcp::socket & sock)
{
    asio::streambuf buf;
    asio::read_until(sock, buf, '\n');

    std::string message;
    // Because buffer 'buf' may contain some other data
    // after '\n' symbol, we have to parse the buffer and
    // extract only symbols before the delimiter.
    std::istream input_stream(&buf);
    std::getline(input_stream, message);

    return message;
}

int main(){

    // client reading from socket
    std::string raw_ip_address = "127.0.0.1";
    u_short port_num = 8002;

    try {
        asio::ip::tcp::endpoint
            ep(asio::ip::address::from_string(raw_ip_address), port_num);

        asio::io_service ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);

        auto res = read_from_socket_read(sock);
        std::cout << res << std::endl;
    }
    catch (system::system_error & e){
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();
        return e.code().value();
    }


    return 0;
}
