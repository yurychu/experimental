
#include <RunCommon.hpp>

#include <string>
#include <memory>
#include <iostream>

#include <boost/asio.hpp>
namespace asio = boost::asio;


void run_common_1()
{
    // prepare buffer for output operations
    // send() or write()

    const std::string buf {"Hello"};

    asio::const_buffers_1 output_buf = asio::buffer(buf);
    // output_buf can be used for output operations.
}

void run_common_2()
{
    // prepare buffer for input operations
    // receive() or read()
    const size_t BUF_SIZE_BYTES = 30;
    auto buf = std::make_unique<char []>(BUF_SIZE_BYTES);

    asio::mutable_buffers_1 input_buf = asio::buffer(buf.get(), BUF_SIZE_BYTES);
    // input_buf can be used for input operations
}

void run_common_3()
{
    asio::streambuf buf;

    std::ostream out_buf{&buf};
    out_buf << "Message1\nMessage2\n";

    std::istream in_buf{&buf};
    std::string message1;
    std::getline(in_buf, message1);
    std::cout << "Message1: " << message1 << std::endl;

    std::string message2;
    std::getline(in_buf, message2);
    std::cout << "Message2: " << message2 << std::endl;


}

