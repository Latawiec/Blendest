#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "public/Network/Error.hpp"

namespace Network {

class HttpSessionPayload {
    const std::string              _host, _port;
    boost::asio::thread_pool       _ioc{1};
    boost::beast::tcp_stream       _stream{_ioc};
    boost::beast::flat_buffer      _buffer;

    bool _stop_requested = false;

public:
    HttpSessionPayload(const std::string& host, const std::string& port);

    std::future<Error> GetFile(const std::string& target, const std::string& outputFilePath);
    std::future<Error> SendFile(const std::string& target, const std::string& inputFilePath);

private:

    std::future<Error> do_getFile(const std::string& target, const std::string& outputFilePath);
    std::future<Error> do_sendFile(const std::string& target, const std::string& inputFilePath);
};

}