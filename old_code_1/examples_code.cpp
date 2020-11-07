#include <cstdint>
#include <iostream>

#include <boost/asio.hpp>

/*
 * Debugging requests
 *
 * tcp
 * ncat -l --keep-open 8002 --recv-only > /home/yurychu/tmp.txt
 *
 * udp
 * ncat -l 8002 --keep-open --udp --sh-exec "cat > /dev/null"
 *
 */
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



int main6(){

    // binding to endpoint, all available
    u_short port_num = 3333;
    asio::ip::tcp::endpoint ep(
            asio::ip::address_v4::any(),
            port_num
    );

    asio::io_service ios;
    asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

    boost::system::error_code ec;

    acceptor.bind(ep, ec);
    if (ec.value() != 0) {
        std::cout << "Failed to bind the acceptor socket."
                  << "Error code = " << ec.value() << ". Message: "
                  << ec.message();
        return ec.value();
    }

    return 0;
}


int main7(){

    // udp server
    u_short port_num = 3333;
    asio::ip::udp::endpoint ep(
            asio::ip::address_v4::any(),
            port_num
    );

    asio::io_service ios;
    asio::ip::udp::socket sock(ios, ep.protocol());

    boost::system::error_code ec;

    sock.bind(ep, ec);
    if (ec.value() != 0) {
        std::cout << "Failed to bind the socket."
                  << "Error code = " << ec.value() << ". Message: "
                  << ec.message();
        return ec.value();
    }

    return 0;
}

int main8(){
    // tcp client to active socket

    std::string raw_ip_address = "127.0.0.1";
    u_short port_num = 8002;

    try {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
        asio::io_service ios;

        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);

    }
    catch (system::system_error & e){
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();
        return e.code().value();
    }

    return 0;
}


int main9(){
    // listening incoming on server

    const int BACKLOG_SIZE = 30;

    u_short port_num = 8002;

    asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

    asio::io_service ios;

    try {
        asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
        acceptor.bind(ep);

        acceptor.listen(BACKLOG_SIZE);

        asio::ip::tcp::socket sock(ios);

        acceptor.accept(sock); // blocking

    }
    catch (system::system_error & e){
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();
        return e.code().value();
    }


    return 0;
}


int main10(){
    // const buffers send hello

    const std::string buf = "Hello";

    asio::const_buffers_1 output_buf = asio::buffer(buf);

    return 0;
}


int main11(){
    // preparing buffer to receiving data
    const size_t BUF_SIZE_BYTES = 20;

    std::unique_ptr<char[]> buf(new char[BUF_SIZE_BYTES]);

    asio::mutable_buffers_1 input_buf =
            asio::buffer(static_cast<void*>(buf.get()), BUF_SIZE_BYTES);

    // input_buf can be used in asio


    return 0;
}


int main12(){
    // streams examples

    asio::streambuf buf;

    std::ostream output(&buf);

    output << "Mesaage1\nMessage2\n";

    std::istream input(&buf);
    std::string message;
    std::getline(input, message);

    std::cout << message << std::endl;  // contains Message1

    return 0;
}



void write_to_socket(asio::ip::tcp::socket & sock)
{
    std::string buf = "Hello";

    std::size_t total_bytes_written = 0;

    while (total_bytes_written != buf.length()){
        total_bytes_written += sock.write_some(  // garantee min one byte if buf not empty
                asio::buffer(buf.c_str() + total_bytes_written, buf.length() - total_bytes_written)
        );
    }
}


int main13(){
    // client writing to socket
    std::string raw_ip_address = "127.0.0.1";
    u_short port_num = 8002;

    try {
        asio::ip::tcp::endpoint
                ep(asio::ip::address::from_string(raw_ip_address), port_num);

        asio::io_service ios;
        asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
        write_to_socket(sock);
    }
    catch (system::system_error & e){
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();
        return e.code().value();
    }

    return 0;
}


float doMultiplication (float num1, float num2 ) {
    return num1 * num2; }

typedef float(pt2Func)(float, float);

pt2Func *myFnPtr = doMultiplication;

float result = myFnPtr(2.0, 5.1);


class Storage
{
private:
    std::map<int, std::string> itsStorage;

public:
    Storage(){
        std::cout << "Create storage" << std::endl;
        itsStorage[1] = "Resource One";
        itsStorage[2] = "Resource Two";
        itsStorage[3] = "Resource Three";
        itsStorage[4] = "Resource Four";
        itsStorage[5] = "Resource Five";
    }
    ~Storage() = default;

    std::string get(u_int id_resource)
    {
        std::string result;
        auto it = itsStorage.find(id_resource);

        if (it != itsStorage.end()){
            result = it->second;
        }
        else {
            result = "Not found resource with id: " + std::to_string(id_resource);
        }

        return result;
    }

};
