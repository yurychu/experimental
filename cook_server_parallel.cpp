#include <cstdint>
#include <iostream>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

#include <boost/predef.h> // Tools to identify the OS.


#ifdef BOOST_OS_WINDOWS
# define _WIN32_WINNT 0x0501
# if _WIN32_WINNT <= 0x0502 // Windows Server 2003 or earlier.
#  define BOOST_ASIO_DISABLE_IOCP
#  define BOOST_ASIO_ENABLE_CANCELIO
# endif
#endif

#include <boost/chrono.hpp>
#include <boost/thread.hpp>


using namespace boost;


class Service
{
public:
    Service() = default;

    void start_handle_client(std::shared_ptr<asio::ip::tcp::socket> sock)
    {
        std::thread th(
                [this, sock](){
                    this->handle_client(sock);
                }
        );
        th.detach();
    }

private:

    void handle_client(std::shared_ptr<asio::ip::tcp::socket> sock)
    {
        try {
            asio::streambuf request;
            asio::read_until(*sock.get(), request, '\n');

            int i = 0;
            while (i != 10 * 6){
                i++;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // sending response
            std::string response = "Response\n";
            asio::write(*sock.get(), asio::buffer(response));
        }
        catch (system::system_error & e){
            std::cout    << "Error occured! Error code = "
                         << e.code() << ". Message: "
                         << e.what();
        }
        delete this;
    }

};


class Acceptor
{
public:
    Acceptor(asio::io_service & ios ,u_short port_num)
            : m_ios(ios),
              m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num))
    {
        m_acceptor.listen();
    }

    void accept()
    {
        std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(m_ios));

        m_acceptor.accept(*sock.get());

        (new Service)->start_handle_client(sock);
    }


private:
    asio::io_service & m_ios;
    asio::ip::tcp::acceptor m_acceptor;
};


class Server
{
public:
    Server()
            : m_stop(false)
    {

    }

    void start(u_short port_num)
    {
        m_thread.reset(
                new std::thread(
                        [this, port_num]()
                        {
                            this->run(port_num);
                        }
                )
        );
        std::cout << "Server start method end" << std::endl;
    }

    void stop()
    {
        m_stop.store(true);
        m_thread->join();
    }

private:

    void run(u_short port_num)
    {
        Acceptor acc(m_ios, port_num);

        while(!m_stop.load()){
            std::cout << "Start Cycle iteration" << std::endl;
            acc.accept();
            std::cout << "End cycle iteration" << std::endl;
        }

    }

private:
    std::unique_ptr<std::thread> m_thread;
    std::atomic<bool> m_stop;
    asio::io_service m_ios;
};



int main()
{
    std::cout << "Server starting..." << std::endl;

    u_short port_num = 8003;

    try {
        Server srv;
        srv.start(port_num);

        std::cout << "Sleeping" << std::endl;
        boost::this_thread::sleep_for(boost::chrono::seconds(30));
        std::cout << "End sleep... stopping server..." << std::endl;

        srv.stop();
    }
    catch (system::system_error & e){
        std::cout  << "Error occured! Error code = "
                   <<e.code() << ". Message: "
                   <<e.what();
    }

    return 0;
};
