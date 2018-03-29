#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>


namespace asio = boost::asio;


using ipc_protocol = boost::asio::local::stream_protocol;


using HeaderPartBufType = std::array<char, 8>;


using SomeBufType = std::array<char, 16>;


constexpr u_char START_WORD_LEN_BYTES = 8;
constexpr u_char META_DATA_LEN_BYTES = 8;
constexpr u_char DATA_LEN_BYTES = 8;
constexpr u_char SUM_LENGHT_BYTES = START_WORD_LEN_BYTES + META_DATA_LEN_BYTES + DATA_LEN_BYTES;

using HeaderBufferType = std::array<char, SUM_LENGHT_BYTES>;

class Header
{
private:
    HeaderBufferType itsBuffer;

public:
    Header() = default;
    ~Header() = default;

    HeaderBufferType & get_buffer()
    {
        return itsBuffer;
    }

    size_t get_metadata_size() const
    {
        std::string res(itsBuffer.data() + START_WORD_LEN_BYTES, META_DATA_LEN_BYTES);
        return std::stoul(res);
    }

    size_t get_data_size() const
    {
        std::string res(itsBuffer.data() + START_WORD_LEN_BYTES + META_DATA_LEN_BYTES, DATA_LEN_BYTES);
        return std::stoul(res);
    }

};


using BodyBufferType = std::array<char, (8 << 17)>;  /* (8 << 17) 1048576 */

class Body
{
private:
    BodyBufferType itsBuffer;
    size_t itsMetadataSize;
    size_t itsDataSize;

public:
    Body()
    : itsBuffer(),
      itsMetadataSize(0),
      itsDataSize(0)
    {
    }

    explicit Body(const Header & header)
            : itsBuffer(),
              itsMetadataSize(header.get_metadata_size()),
              itsDataSize(header.get_data_size())
    {
    }

    void set_header(const Header & header)
    {
        itsMetadataSize = header.get_metadata_size();
        itsDataSize = header.get_data_size();
    }

    size_t get_sum_size() const
    {
        return itsMetadataSize + itsDataSize;
    }

    BodyBufferType & get_buffer()
    {
        return itsBuffer;
    }

    std::string get_metadata() const
    {
        return std::string(itsBuffer.data(), itsMetadataSize);
    }

    std::string get_data() const
    {
        return std::string(itsBuffer.data() + itsMetadataSize, itsDataSize);
    }

    ~Body() = default;
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

    Header itsHeader;
    Body itsBody;

    SomeBufType itsSomeBuf;

public:
    explicit Session(std::shared_ptr<ipc_protocol::socket> socket, std::shared_ptr<Storage> storage)
            : itsStorage(storage),
              itsSocket(socket)
    {
        std::cout << "Created new session" << std::endl;
    }

    void start_communicate()
    {
       this->read_header();
    }

private:

    void read_header()
    {
        auto self(shared_from_this());
        asio::async_read(*itsSocket, asio::buffer(itsHeader.get_buffer()),
                         [this, self]
                                 (const boost::system::error_code & ec,
                                  std::size_t bytes_transferred)
                         {
                             this->on_read_header(ec, bytes_transferred);
                         });
    }

    void on_read_header(const boost::system::error_code & ec,
                        std::size_t bytes_transferred
    ){
        if (!ec.value()) {
            std::cout << "Header read bytes transferred: " << bytes_transferred << std::endl
                      << "Metadata size: " << itsHeader.get_metadata_size() << std::endl
                      << "Data size: " << itsHeader.get_data_size() << std::endl;

            itsBody.set_header(itsHeader);

            auto self(shared_from_this());
            asio::async_read(*itsSocket, asio::buffer(itsBody.get_buffer(), itsBody.get_sum_size()),
                             [this, self]
                                     (const boost::system::error_code & ec,
                                      std::size_t bytes_transferred)
                             {
                                 this->on_read_body(ec, bytes_transferred);
                             });
        }
        else {
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
        }
    }

    void on_read_body(const boost::system::error_code & ec,
                        std::size_t bytes_transferred){
        if (!ec.value()) {
            std::cout << "Body read bytes transferred: " << bytes_transferred << std::endl
                      << "Metadata: " << itsBody.get_metadata() << std::endl
                      << "Data: " << itsBody.get_data() << std::endl;

            this->read_header();
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