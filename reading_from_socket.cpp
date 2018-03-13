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

int main_sync(){

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


struct SessionSome
{
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::unique_ptr<char []> buf;
    std::size_t total_bytes_read;
    u_int buf_size;
};

void read_some_callback(const boost::system::error_code & ec,
                        std::size_t bytes_transferred,
                        std::shared_ptr<SessionSome> s)
{
    if (ec.value() != 0){
        std::cout << "Error occured! Error code = "
                  << ec.value()
                  << ". Message: " << ec.message();
        return;
    }

    s->total_bytes_read += bytes_transferred;

    if (s->total_bytes_read == s->buf_size){
        std::cout << s->buf.get() << std::endl;
        return;
    }

    s->sock->async_read_some(
            asio::buffer(
                    s->buf.get() + s->total_bytes_read,
                    s->buf_size - s->total_bytes_read
            ),
            std::bind(read_some_callback, std::placeholders::_1, std::placeholders::_2, s)
    );
}

void read_some_from_socket(std::shared_ptr<asio::ip::tcp::socket> sock)
{
    std::shared_ptr<SessionSome> s(new SessionSome);
    const u_int MESSAGE_SIZE = 7;

    s->buf.reset(new char[MESSAGE_SIZE]);
    s->total_bytes_read = 0;
    s->sock = sock;
    s->buf_size = MESSAGE_SIZE;

    s->sock->async_read_some(
            asio::buffer(s->buf.get(), s->buf_size),
            std::bind(
                    read_some_callback,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    s
            )
    );
}

int main()
{
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
        read_some_from_socket(sock);
        ios.run();
    }
    catch (boost::system::system_error & e){
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();
        return e.code().value();
    }

    return 0;
}