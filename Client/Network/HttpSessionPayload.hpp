#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "public/Network/Error.hpp"

namespace Network {

class HttpSessionPayload {
    const std::string              _host;
    boost::asio::thread_pool       _ioc{1};
    boost::beast::tcp_stream       _stream{_ioc};
    boost::beast::flat_buffer      _buffer;

    bool _stop_requested = false;

public:
    HttpSessionPayload(const std::string& host);
    ~HttpSessionPayload();

    void Start();
    void Stop();

    std::future<Error> GetFile(const std::string& target, const std::string& outputFilePath);

private:
    void do_connect();
    std::future<void> do_stop();

    std::future<Error> do_getFile(const std::string& target, const std::string& outputFilePath);
};

}