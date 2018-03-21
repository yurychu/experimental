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
    Service(std::shared_ptr<asio::ip::tcp::socket> sock)
            : m_sock(sock){}

    void start_handling()
    {
        asio::async_read_until(*m_sock.get(), m_request, '\n',
                               [this](
                                       const boost::system::error_code & ec,
                                       std::size_t bytes_transferred
                               ){
                                   this->on_request_received(ec, bytes_transferred);
                               }
        );
    }

private:
    void on_request_received(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {
        if (ec.value() != 0){
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
            this->on_finish();
            return;
        }

        m_response = this->process_request(m_request);

        asio::async_write(*m_sock.get(), asio::buffer(m_response),
                          [this](
                                  const boost::system::error_code & ec,
                                  std::size_t bytes_transferred
                          ){
                              this->on_response_sent(ec, bytes_transferred);
                          }
        );
    }

    void on_response_sent(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {
        if (ec.value() != 0){
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
        }
        this->on_finish();
    }

    void on_finish()
    {
        delete this;
    }

    std::string process_request(asio::streambuf & request)
    {
        // parse request
        int i = 0;
        while (i != 10*6){
            i++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::string response = "Response\n";
        return response;
    }

private:
    std::shared_ptr<asio::ip::tcp::socket> m_sock;
    std::string m_response;
    asio::streambuf m_request;
};


class Acceptor
{
public:
    Acceptor(asio::io_service & ios, u_short port_num)
            : m_ios(ios),
              m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)),
              m_is_stopped(false)
    {

    }

    void start()
    {
        m_acceptor.listen();
        this->init_accept();
    }

    void stop()
    {
        m_is_stopped.store(true);
    }

private:
    void init_accept()
    {
        std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(m_ios));
        m_acceptor.async_accept(*sock.get(),
                                [this, sock](
                                        const boost::system::error_code & ec
                                ){
                                    this->on_accept(ec, sock);
                                }
        );
    }

    void on_accept(const boost::system::error_code & ec, std::shared_ptr<asio::ip::tcp::socket> sock)
    {
        if (ec.value() == 0){
            (new Service(sock))->start_handling();
        }
        else {
            std::cout<< "Error occured! Error code = "
                     << ec.value()
                     << ". Message: " <<ec.message();
        }

        if (!m_is_stopped.load()){
            this->init_accept();
        }
        else {
            m_acceptor.close();
        }
    }


private:
    asio::io_service & m_ios;
    asio::ip::tcp::acceptor m_acceptor;
    std::atomic<bool> m_is_stopped;
};



class Server
{
public:
    Server(){
        m_work.reset(new asio::io_service::work(m_ios));
    }

    void start(u_short port_num, u_int thread_pool_size)
    {
        assert(thread_pool_size > 0);

        acc.reset(new Acceptor(m_ios, port_num));
        acc->start();

        for (u_int i = 0; i < thread_pool_size; i++){
            std::unique_ptr<std::thread> th(
                    new std::thread(
                            [this](){
                                m_ios.run();
                            }
                    )
            );
            m_thread_pool.push_back(std::move(th));
        }

    }

    void stop()
    {
        acc->stop();
        m_ios.stop();

        for (auto & th : m_thread_pool){
            th->join();
        }
    }

private:
    asio::io_service m_ios;
    std::unique_ptr<asio::io_service::work> m_work;
    std::unique_ptr<Acceptor> acc;
    std::vector<std::unique_ptr<std::thread>> m_thread_pool;
};


const u_int DEFAULT_THREAD_POOL_SIZE = 2;

int main()
{
    u_short port_num = 8003;

    try {
        Server srv;

        u_int thread_pool_size = std::thread::hardware_concurrency() * 2;
        if (thread_pool_size == 0){
            thread_pool_size = DEFAULT_THREAD_POOL_SIZE;
        }
        srv.start(port_num, thread_pool_size);

        std::this_thread::sleep_for(std::chrono::seconds(60));
        srv.stop();
    }
    catch (system::system_error & e) {
        std::cout  << "Error occured! Error code = "
                   <<e.code() << ". Message: "
                   <<e.what();
    }

    return 0;
};
