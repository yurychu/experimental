#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>

#include <boost/asio.hpp>


using namespace boost;


class SyncTCPClient
{
public:
    SyncTCPClient(const std::string & raw_ip_address, u_short port_num)
            : m_ep(asio::ip::address::from_string(raw_ip_address), port_num),
              m_sock(m_ios)
    {
        m_sock.open(m_ep.protocol());
    }

    void connect(){
        m_sock.connect(m_ep);
    }

    void close(){
        m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        m_sock.close();
    }

private:
    asio::io_service m_ios;
    asio::ip::tcp::endpoint m_ep;
    asio::ip::tcp::socket m_sock;
};