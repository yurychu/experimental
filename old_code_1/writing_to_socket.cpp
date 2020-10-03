#include <cstdint>
#include <iostream>
#include <memory>

#include <boost/asio.hpp>


using namespace boost;

void write_to_socket(asio::ip::tcp::socket & sock)
{
    std::string buf = "Hello";

    std::size_t total_bytes_written = 0;

    while (total_bytes_written != buf.length()){
        total_bytes_written += sock.write_some(
                asio::buffer(buf.c_str() + total_bytes_written, buf.length() - total_bytes_written)
        );
    }

}

void write_to_socket_write(asio::ip::tcp::socket & sock)
{
    std::string buf = "Hello2";
    asio::write(sock, asio::buffer(buf));
}


int main_sync(){
    // client writing to socket
    std::string raw_ip_address = "127.0.0.1";
    u_short port_num = 8002;

    try {
        asio::ip::tcp::endpoint
            ep(asio::ip::address::from_string(raw_ip_address), port_num);

        asio::io_service ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
        write_to_socket_write(sock);
    }
    catch (system::system_error & e){
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();
        return e.code().value();
    }

    return 0;
}


struct SessionSome
{
    std::shared_ptr <asio::ip::tcp::socket> sock;
    std::string buf;
    std::size_t total_bytes_written;
};


struct SessionSmallerWrite
{
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::string buf;
};


void write_handler_callback(
        const boost::system::error_code ec,
        std::size_t bytes_transferred,
        std::shared_ptr<SessionSmallerWrite> s
)
{
    if (ec.value() != 0){
        std::cout << "Error occured! Error code = "
                  << ec.value()
                  << ". Message: " << ec.message();
        return;
    }
    // here are good and all data has been written to sock
}


void write_some_handler_callback(const boost::system::error_code &ec, std::size_t bytes_transferred,
                                 std::shared_ptr<SessionSome> s)
{
    if (ec.value() != 0){
        std::cout << "Error occured! Error code = "
                  << ec.value()
                  << ". Message: " << ec.message();
        return;
    }

    s->total_bytes_written += bytes_transferred;
    if (s->total_bytes_written == s->buf.length()){
        return;
    }

    s->sock->async_write_some(
            asio::buffer(
                    s->buf.c_str() + s->total_bytes_written, s->buf.length() - s->total_bytes_written
            ),
            std::bind(write_some_handler_callback, std::placeholders::_1, std::placeholders::_2, s)
    );
}


void write_some_to_socket_async(std::shared_ptr<asio::ip::tcp::socket> sock)
{
    std::shared_ptr<SessionSome> s(new SessionSome);

    s->buf = std::string("Hi there!!!");
    s->total_bytes_written = 0;
    s->sock = sock;

    s->sock->async_write_some(
            asio::buffer(s->buf),
            std::bind(write_some_handler_callback, std::placeholders::_1, std::placeholders::_2, s)
    );
}

int main(){
    // async version
    std::string raw_ip_address = "127.0.0.1";
    u_short port_num = 8003;

    try {
        asio::ip::tcp::endpoint
            ep(asio::ip::address::from_string(raw_ip_address), port_num);

        asio::io_service ios;

        std::shared_ptr<asio::ip::tcp::socket> sock(
                new asio::ip::tcp::socket(ios, ep.protocol())
        );
        sock->connect(ep);
        write_some_to_socket_async(sock);
        ios.run();
    }
    catch (system::system_error & e){
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();
        return e.code().value();
    }

    return 0;
}
