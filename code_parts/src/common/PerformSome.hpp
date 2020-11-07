#pragma once

#include <string>

#include <boost/asio.hpp>

namespace asio = boost::asio;

// sock must be connected, else fail
void write_to_sock(asio::ip::tcp::socket &sock, const std::string &str);