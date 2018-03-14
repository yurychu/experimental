#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>

#include <boost/predef.h> // Tools to identify the OS.

#include <boost/asio.hpp>

#ifdef BOOST_OS_WINDOWS
# define _WIN32_WINNT 0x0501
# if _WIN32_WINNT <= 0x0502 // Windows Server 2003 or earlier.
#  define BOOST_ASIO_DISABLE_IOCP
#  define BOOST_ASIO_ENABLE_CANCELIO
# endif
#endif


using namespace boost;

int main1(){
    // cancelling
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 8003;
    try {
        asio::ip::tcp::endpoint
                ep(asio::ip::address::from_string(raw_ip_address),
                   port_num);
        asio::io_service ios;

        std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(ios, ep.protocol()));

        sock->async_connect(
                ep,
                [sock](const boost::system::error_code & ec){
                    std::cout << "Enter in lambda" << std::endl;
                    if (ec.value() != 0) {
                        if (ec.value() == asio::error::operation_aborted) {
                            std::cout << "Operation cancelled!";
                        }
                        else {
                            std::cout << "Error occured!"
                                      << " Error code = "
                                      << ec.value()
                                      << ". Message: "
                                      << ec.message() << std::endl;
                        }
                        return;
                    }
                    // socket connected this
                    std::cout << "socket connected this" << std::endl;
                }
        );

        std::thread worker_thread(
                [&ios](){
                    try {
                        std::cout << "Ios thread running!" << std::endl;
                        ios.run();
                    }
                    catch (system::system_error & e){
                        std::cout << "Error occured!"
                                  << " Error code = " << e.code()
                                  << ". Message: " << e.what();
                    }
                }
        );

        std::this_thread::sleep_for(std::chrono::seconds(10));

        std::cout << "Make cancel" << std::endl;
        sock->cancel();

        std::cout << "Start join" << std::endl;
        worker_thread.join();
    }
    catch (boost::system::system_error & e){
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();
        return e.code().value();
    }

    return 0;
}

void communicate(asio::ip::tcp::socket & sock)
{
    const char request_buf[] = { 0x48, 0x65, 0x0, 0x6c, 0x6c, 0x6f };

    asio::write(sock, asio::buffer(request_buf));
    sock.shutdown(asio::socket_base::shutdown_send);
    asio::write(sock, asio::buffer(request_buf));

    // extensible buffer for response
    asio::streambuf response_buf;

    system::error_code ec;
    asio::read(sock, response_buf, ec);

    if (ec == asio::error::eof) {
        // Whole response message has been received.
        // Here we can handle it.
    }
    else {
        throw system::system_error(ec);
    }
}

void process_request(asio::ip::tcp::socket & sock)
{
    asio::streambuf request_buf;
    system::error_code ec;

    asio::read(sock, request_buf, ec);
    if (ec.value() != asio::error::eof){
        std::cout << "Throwing!!!" << std::endl;
        throw system::system_error(ec);
    }
    const char response_buf[] = { 0x48, 0x69, 0x21 };
    asio::write(sock, asio::buffer(response_buf));
    sock.shutdown(asio::socket_base::shutdown_send);
}

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

template <typename T, size_t N>
void function_array_size( T (&array)[N] )
{
    size_t myarraysize = N;
    std::cout << myarraysize << std::endl;
}


int main()
{
    //client
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 8003;
    try {
        asio::ip::tcp::endpoint
                ep(asio::ip::address::from_string(raw_ip_address),
                   port_num);
        asio::io_service ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
        communicate(sock);
    }
    catch (system::system_error &e) {
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();
        return e.code().value();
    }
    return 0;
}

int main_serv(){
    // server
    unsigned short port_num = 8003;
    try {
        asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(),
                                   port_num);
        asio::io_service ios;
        asio::ip::tcp::acceptor acceptor(ios, ep);
        asio::ip::tcp::socket sock(ios);
        acceptor.accept(sock);
        process_request(sock);
    }
    catch (system::system_error &e) {
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();
        return e.code().value();
    }
    return 0;

}
