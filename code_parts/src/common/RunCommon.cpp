
#include <RunCommon.hpp>

#include <string>
#include <memory>

#include <boost/asio.hpp>
namespace asio = boost::asio;


void run_common_1()
{
    // prepare buffer for output operations
    // send() or write()

    const std::string buf {"Hello"};

    asio::const_buffers_1 output_buf = asio::buffer(buf);
    // output_buf can be used.
}

void run_common_2()
{
    // prepare buffer for input operations
    // receive() or read()
    const size_t BUF_SIZE_BYTES = 30;
    auto buf = std::make_unique<char []>(BUF_SIZE_BYTES);

    asio::mutable_buffers_1 input_buf = asio::buffer(buf.get(), BUF_SIZE_BYTES);

}
