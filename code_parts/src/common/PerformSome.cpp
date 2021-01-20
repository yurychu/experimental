
#include <PerformSome.hpp>


void write_to_sock(asio::ip::tcp::socket &sock, const std::string &str)
{
    std::size_t total_bytes_written = 0;

    while (total_bytes_written < str.size()){
        // гарантирует, что хотя бы один байт будет записан, если не случится какой то ошибки.
        total_bytes_written += sock.write_some(  // send() member function have synonym overload write_some (p68)
                asio::buffer(
                        str.c_str() + total_bytes_written,
                        str.size() - total_bytes_written
                )
        );
    }

}

void write_to_sock_enhanced(asio::ip::tcp::socket &sock, const std::string &str)
{
    asio::write(sock, asio::buffer(str));  // attempt write all data in the buffer to the socket object.
}


std::string read_from_sock(asio::ip::tcp::socket &sock)
{
    const uint8_t MESSAGE_SIZE = 7;
    char buf [MESSAGE_SIZE];
    size_t total_bytes_read = 0;

    while (total_bytes_read < MESSAGE_SIZE){
        total_bytes_read += sock.read_some(  // receive is equivalent
                asio::buffer(buf + total_bytes_read, MESSAGE_SIZE - total_bytes_read)
        );
    }

    std::string::value_type
    return std::string { buf, MESSAGE_SIZE };
}

std::string read_from_sock_enhanced(asio::ip::tcp::socket &sock)
{
    const uint8_t MESSAGE_SIZE = 7;
    char buf [MESSAGE_SIZE];

    asio::read(sock, asio::buffer(buf, MESSAGE_SIZE));

    return std::string();
}

