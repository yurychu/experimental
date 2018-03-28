#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>


namespace asio = boost::asio;


using ipc_protocol = boost::asio::local::stream_protocol;


using HeaderPartBufType = std::array<char, 8>;
using BodyPartBufType = std::array<char, (8 << 17)>;  /* (8 << 17) 1048576 */

using SomeBufType = std::array<char, 16>;


class Part
{
private:
    HeaderPartBufType itsHeaderBuf;
    BodyPartBufType itsBodyBuf;

public:
    Part() = default;
    ~Part() = default;

    HeaderPartBufType & get_header_buf()
    {
        return itsHeaderBuf;
    }

    BodyPartBufType & get_body_buf()
    {
        return itsBodyBuf;
    }

    size_t get_length_for_read()
    {
        std::string str(itsHeaderBuf.data(), itsHeaderBuf.size());
        return std::stoul(str);
    }

};


class Packet
{
private:
    Part itsMetaData;
    Part itsData;

public:
    Packet() = default;
    ~Packet() = default;

    Part& meta_data_part(){
        return itsMetaData;
    }

    Part& data_part(){
        return itsData;
    }

};


class TaskPacker
{
private:
    bool itsHereClear;

public:
    TaskPacker()
            :itsHereClear(true)
    {

    }
    ~TaskPacker() = default;

    void eat(const SomeBufType & some_buf)
    {
        std::string meta_data_len(some_buf.data(), 4);
        std::string data_len(some_buf.data()+4, 4);

        std::cout << "Metadata len: " << meta_data_len
                  << "Data len: " << data_len
                  << std::endl;

    }
};

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


class Session
        : public std::enable_shared_from_this<Session>
{
private:
    std::shared_ptr<Storage> itsStorage;
    std::shared_ptr<ipc_protocol::socket> itsSocket;

    std::unique_ptr<TaskPacker> itsTaskPacker;

    SomeBufType itsSomeBuf;

public:
    explicit Session(std::shared_ptr<ipc_protocol::socket> socket, std::shared_ptr<Storage> storage)
            : itsStorage(storage),
              itsSocket(socket),
              itsTaskPacker(std::make_unique<TaskPacker>())
    {
        std::cout << "Created new session" << std::endl;
    }

    void start_communicate()
    {
       this->fill_the_buffer();
    }

private:

    void fill_the_buffer()
    {
        auto self(shared_from_this());
        asio::async_read(*itsSocket, asio::buffer(itsSomeBuf),
                         [this, self]
                                 (const boost::system::error_code & ec,
                                  std::size_t bytes_transferred)
                         {
                             this->parse_to_packet(ec, bytes_transferred);
                         });
    }


    void parse_to_packet(const boost::system::error_code & ec, std::size_t bytes_transferred){
        if (!ec.value()) {
            std::cout << "Bytes Transferred: " << bytes_transferred << std::endl;
            std::cout << std::string(itsSomeBuf.data(), itsSomeBuf.size()) << std::endl;

            itsTaskPacker->eat(itsSomeBuf);

            this->fill_the_buffer();
        }
        else {
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
        }

    }

};


class Acceptor
{
private:
    std::shared_ptr<Storage> itsStorage;

    asio::io_service & itsIos;
    ipc_protocol::acceptor itsAcceptor;

public:
    Acceptor(asio::io_service & ios, std::string acc_name)
    : itsStorage(nullptr),
      itsIos(ios),
      itsAcceptor(ios)
    {
        if(acc_name.find('.') == std::string::npos){
            acc_name += ".socket";
        }
        ::unlink(acc_name.c_str());
        ipc_protocol::endpoint ep(acc_name);

        itsAcceptor.open(ep.protocol());
        itsAcceptor.bind(ep);
    }

    void start()
    {
        itsAcceptor.listen();
        this->init_accept();
    }

    void set_storage(std::shared_ptr<Storage> storage)
    {
        itsStorage = storage;
    }

private:
    void init_accept()
    {
        auto sock = std::make_shared<ipc_protocol::socket>(itsIos);
        itsAcceptor.async_accept(*sock,
                                 [this, sock]
                                         (const boost::system::error_code & ec)
                                 {
                                     this->on_accept(ec, sock);
                                 });
    }

    void on_accept(const boost::system::error_code & ec, std::shared_ptr<ipc_protocol::socket> sock)
    {
        std::cout << "on_accept" << std::endl;
        if (!ec.value()){
            auto session = std::make_shared<Session>(sock, itsStorage);
            session->start_communicate();

            // may be check if cancelling
            this->init_accept();
        }
        else {
            std::cout<< "Error occured! Error code = "
                     << ec.value()
                     << ". Message: " <<ec.message();
        }



    }

};


class Server
{
private:
    asio::io_service & itsIos;
    Acceptor itsAcceptor;

    std::shared_ptr<Storage> itsStorage;

public:
    explicit Server(asio::io_service & ios, const std::string & ep_name)
            : itsIos(ios),
              itsAcceptor(ios, ep_name),
              itsStorage(nullptr)
    {
    }

    void set_storage(std::shared_ptr<Storage> storage){
        itsStorage = storage;
        itsAcceptor.set_storage(storage);
    }

    void start(){
        itsAcceptor.start();
    }

    ~Server() = default;

};

class Client
{
private:
    asio::io_service & itsIos;

public:
    explicit Client(asio::io_service & ios)
            : itsIos(ios)
    {
    }

    ~Client() = default;

};

class Module
{
private:
    std::shared_ptr<Storage> itsStorage;

    asio::io_service itsIos;
    Server itsServer;
    Client itsClient;

public:
    explicit Module(const std::string & name)
    : itsStorage(std::make_shared<Storage>()),
      itsServer(itsIos, name),
      itsClient(itsIos)
    {
        itsServer.set_storage(itsStorage);
        itsServer.start();
    };

    ~Module() = default;

    void start_ios(){
        itsIos.run();
    }

    void stop_ios(){
        itsIos.stop();
    }
};


int main()
{
    Module module("module_one");

    module.start_ios();

    return 0;
}