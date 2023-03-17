#pragma once

#include <string>
#include <chrono>
#include <mutex>
#include <vector>
#include <algorithm>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace Connection {

class HandlerPayload {

    using ErrorCallbackT = std::function<void(const boost::system::error_code&)>;
    using MessageCallbackT = std::function<void(const boost::beast::flat_buffer&)>;
    
    std::mutex _onErrorLock;
    ErrorCallbackT _onError;

    std::mutex _onMessageLock;
    MessageCallbackT _onMessage;

    // Declare Boost stuff as last, its important for destruction order... unfortunately.

    const std::string                     _host, _port;
    boost::beast::flat_buffer             _buffer;
    boost::asio::thread_pool              _ioc{1};
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws{_ioc};
    boost::asio::steady_timer             _timer{_ws.get_executor()};

    std::chrono::milliseconds _tryReconnectTimeout;

    bool _stop_requested = false;
    enum class Status { INIT, CONNECTED, DISCONNECTED, STOPPED } _status{};

public:


    HandlerPayload(const std::string& host, const std::string& port, uint64_t reconnectTimeoutMs);
    ~HandlerPayload();

    void Start();
    void Stop();
    
    void OnError(ErrorCallbackT callback);
    void OnMessage(MessageCallbackT callback);

private:
    void do_step_machine();
    
    void handle_error(const boost::system::error_code& ec);
    void handle_message(const boost::beast::flat_buffer& data);


    void do_connect();
    void do_read();
    void do_reconnect_delay(std::chrono::steady_clock::duration d);

    std::future<void> do_stop();
};

} // namespace Connection