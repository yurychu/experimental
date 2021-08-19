
#include <RunCommon.hpp>

#include <string>
#include <memory>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
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

asio::io_service ios;

void WorkerThread()
{
    std::cout << "Thread Start\n";
    ios.run();
    std::cout << "Thread Finish\n";
}

boost::mutex global_stream_lock;

void WorkerThread(boost::shared_ptr< boost::asio::io_service > io_service)
{

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() <<
              "] Thread Start" << std::endl;
    global_stream_lock.unlock();

    io_service->run();

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() <<
              "] Thread Finish" << std::endl;
    global_stream_lock.unlock();
}

namespace aa{
    void F1()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    void F2(int i, float f)
    {
        std::cout << "i: " << i << std::endl;
        std::cout << "f: " << f << std::endl;
    }

    class MyClass
    {
    public:
        void F3(int i, float f)
        {
            std::cout << "i: " << i << std::endl;
            std::cout << "f: " << f << std::endl;
        }
    };
}


size_t fib( size_t n )
{
    if ( n <= 1 )
    {
        return n;
    }
    boost::this_thread::sleep(
            boost::posix_time::milliseconds( 1000 )
    );
    return fib( n - 1 ) + fib( n - 2);
}

void CalculateFib( size_t n )
{
    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id()
              << "] Now calculating fib( " << n << " ) " << std::endl;
    global_stream_lock.unlock();

    size_t f = fib( n );

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id()
              << "] fib( " << n << " ) = " << f << std::endl;
    global_stream_lock.unlock();
}

void Dispatch( int x )
{
    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] "
              << __FUNCTION__ << " x = " << x << std::endl;
    global_stream_lock.unlock();
}

void Post( int x )
{
    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id() << "] "
              << __FUNCTION__ << " x = " << x << std::endl;
    global_stream_lock.unlock();
}

void Run3( boost::shared_ptr< boost::asio::io_service > io_service )
{
    for( int x = 0; x < 3; ++x )
    {
        io_service->post( boost::bind( &Post, x * 2 + 1 ) );
        io_service->dispatch( boost::bind( &Dispatch, x * 2 ) );
        boost::this_thread::sleep( boost::posix_time::milliseconds( 1000 ) );
    }
}

void run_common_4()
{
    boost::shared_ptr< boost::asio::io_service > io_service(
            new boost::asio::io_service
    );
    boost::shared_ptr< boost::asio::io_service::work > work(
            new boost::asio::io_service::work( *io_service )
    );

    global_stream_lock.lock();
    std::cout << "[" << boost::this_thread::get_id()
              << "] The program will exit when all work has finished." << std::endl;
    global_stream_lock.unlock();

    boost::thread_group worker_threads;
    for( int x = 0; x < 1; ++x )
    {
        worker_threads.create_thread( boost::bind( &WorkerThread, io_service ) );
    }

    io_service->post( boost::bind( &Run3, io_service ) );

    work.reset();

    worker_threads.join_all();

    std::cout << "Hi end" << std::endl;

    asio::deadline_timer timer;
    timer.async_wait()

}

