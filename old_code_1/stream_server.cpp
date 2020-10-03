//
// stream_server.cpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)

using boost::asio::local::stream_protocol;

class session
        : public boost::enable_shared_from_this<session>
{
public:
    session(boost::asio::io_service& io_service)
            : socket_(io_service)
    {
        std::cout << "Create new session" << std::endl;
    }

    stream_protocol::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_),
                                boost::bind(&session::handle_read,
                                            shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred)
    {
        std::cout << "Call handle read" << std::endl;
        if (!error)
        {
            std::vector<uchar> img_mem;
            if (data_[0] == 'A'){
                std::cout << "First letter is A" << std::endl;
                cv::Mat img = cv::Mat::zeros(cv::Size(400, 500), CV_8U);
                cv::rectangle(img, cv::Rect(30, 30, 300, 400), cv::Scalar(255), 2);
                cv::imshow("server", img);
                cv::waitKey(0);
                cv::imencode(".jpg", img, img_mem);
                std::cout << img_mem.size() << std::endl;
            }

            if (img_mem.empty()) {
                boost::asio::async_write(socket_,
                                         boost::asio::buffer("no", 2),
                                         boost::bind(&session::handle_write,
                                                     shared_from_this(),
                                                     boost::asio::placeholders::error));
            }
            else {
                boost::asio::async_write(socket_,
                                         boost::asio::buffer(img_mem, img_mem.size()),
                                         boost::bind(&session::handle_write,
                                                     shared_from_this(),
                                                     boost::asio::placeholders::error));
            }

        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        std::cout << "Call handle write" << std::endl;
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_),
                                    boost::bind(&session::handle_read,
                                                shared_from_this(),
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }
    }

private:
    // The socket used to communicate with the client.
    stream_protocol::socket socket_;

    // Buffer used to store data received from the client.
    boost::array<char, 256> data_;
};

typedef boost::shared_ptr<session> session_ptr;

class server
{
public:
    server(boost::asio::io_service& io_service, const std::string& file)
            : io_service_(io_service),
              acceptor_(io_service, stream_protocol::endpoint(file))
    {
        session_ptr new_session(new session(io_service_));
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&server::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }

    void handle_accept(session_ptr new_session,
                       const boost::system::error_code& error)
    {
        std::cout << "Called handle accept!!!" << std::endl;
        if (!error)
        {
            std::cout << "Called before new_sess start!!!" << std::endl;
            new_session->start();
            std::cout << "Called after new_sess start!!!" << std::endl;
        }

        std::cout << "After check error!!!" << std::endl;
        new_session.reset(new session(io_service_));
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&server::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }

private:
    boost::asio::io_service& io_service_;
    stream_protocol::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
    try
    {
        std::string sock_name = "its_socket.socket";
        boost::asio::io_service io_service;

        std::remove(sock_name.c_str());
        server s(io_service, sock_name);

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

#else // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
# error Local sockets not available on this platform.
#endif // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)