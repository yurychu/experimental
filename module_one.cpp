#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>


namespace asio = boost::asio;


using ipc_protocol = boost::asio::local::stream_protocol;


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

    size_t itsMetadataLength;
    size_t itsDataLength;

    asio::streambuf itsStatusLine;
    asio::streambuf itsMetaData;
    asio::streambuf itsData;

    asio::streambuf itsMessageBuf;

public:
    explicit Session(std::shared_ptr<ipc_protocol::socket> socket, std::shared_ptr<Storage> storage)
            : itsStorage(storage),
              itsSocket(socket)
    {
    }

    void start_communicate()
    {
       this->read_message();
    }

private:

    void read_message(){
        auto self(shared_from_this());
        asio::async_read_until(*itsSocket, itsMessageBuf, "\n\n",
                               [this, self]
                                       (const boost::system::error_code & ec,
                                        std::size_t bytes_transferred)
                               {
                                   this->on_read_message(ec, bytes_transferred);
                               });

    }

    void on_read_message(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {
        if (!ec.value()) {

            std::string str {asio::buffers_begin(itsMessageBuf.data()), asio::buffers_end(itsMessageBuf.data())};

            std::cout << "Readed message: " << str << std::endl;

            this->read_message();
        }
        else {
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
        }
    }

    void read_status_line(){
        auto self(shared_from_this());
        asio::async_read_until(*itsSocket, itsStatusLine, "\n\n",
                               [this, self]
                                       (const boost::system::error_code & ec,
                                        std::size_t bytes_transferred)
                               {
                                   this->on_read_status_line(ec, bytes_transferred);
                               });
    }

    void on_read_status_line(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {
        if (!ec.value()){
            std::istream input(&itsStatusLine);

            std::cout << "Readed status line: " << input.rdbuf() << std::endl;

            std::string check_phrase;
            std::string meta_data_length;
            std::string data_length;

            input >> check_phrase;
            std::cout << "Check phrase: " << check_phrase << std::endl;

            input >> meta_data_length;
            std::cout << "Meta data lenght: " << meta_data_length << std::endl;
            itsMetadataLength = std::stoul(meta_data_length);

            input >> data_length;
            std::cout << "Data lenght: " << data_length << std::endl;
            itsDataLength = std::stoul(data_length);

            input.get();

            this->read_meta_data();
        }
        else {
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
        }
    }

    void read_meta_data()
    {
        std::cout << "itsMetadata: " << itsMetadataLength << std::endl;

        auto self(shared_from_this());
        asio::async_read(*itsSocket, itsMetaData.prepare(itsMetadataLength),
                         [this, self]
                                 (const boost::system::error_code & ec,
                                        std::size_t bytes_transferred)
                               {
                                   this->on_read_metadata(ec, bytes_transferred);
                               });
    }

    void on_read_metadata(const boost::system::error_code & ec, std::size_t bytes_transferred)
    {
        if (!ec.value()) {

            std::string str{asio::buffers_begin(itsMetaData.data()), asio::buffers_end(itsMetaData.data())};

            std::cout << "Readed metadata: " << str << std::endl;

            this->read_message();
        }
        else {
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message();
        }
    }

//    void read_data()
//    {
//        auto self(shared_from_this());
//        asio::async_read(*itsSocket, asio::buffer(itsData, itsDataLength),
//                          [this, self](const boost::system::error_code &ec,
//                                       std::size_t bytes_transferred) {
//                              this->on_read_data(ec, bytes_transferred);
//                          }
//        );
//    }
//
//    void on_read_data(const boost::system::error_code & ec, std::size_t bytes_transferred)
//    {
//        if (!ec.value()){
//            std::cout << std::string(itsData.begin(), itsData.end()) << std::endl;
//            this->read_status_line();
//        }
//        else {
//            std::cout << "Error occured! Error code = "
//                      << ec.value()
//                      << ". Message: " << ec.message();
//        }
//    }

//    void on_response_sent(const boost::system::error_code & ec, std::size_t bytes_transferred)
//    {
//        if (ec.value() != 0){
//            std::cout << "Error occured! Error code = "
//                      << ec.value()
//                      << ". Message: " << ec.message();
//        }
//        else {
//            auto self(shared_from_this());
//            asio::async_read_until(*itsSocket, itsRequest, '\n',
//                                   [this, self]
//                                           (const boost::system::error_code &ec,
//                                            std::size_t bytes_transferred) {
//                                       this->on_request_received(ec, bytes_transferred);
//                                   });
//        }
//    }

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