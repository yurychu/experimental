
#include <PerformSome.hpp>


void write_to_sock(asio::ip::tcp::socket &sock, const std::string &str)
{
    std::size_t total_bytes_written = 0;

    while (total_bytes_written < str.size()){
        total_bytes_written += sock.write_some(  // send() member function have synonym overload write_some (p68)
                asio::buffer(
                        str.c_str() + total_bytes_written,
                        str.size() - total_bytes_written
                )
        );
    }

}
