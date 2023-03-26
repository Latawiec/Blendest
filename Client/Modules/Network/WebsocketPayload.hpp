#pragma once

#include "public/Network/WebsocketStatus.hpp"

#include <string>
#include <chrono>
#include <mutex>
#include <vector>
#include <algorithm>
#include <iostream>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <nod.hpp>

using namespace std::chrono_literals;

namespace Network {

class WebsocketPayload {
    const std::string               _host, _port;

    boost::beast::flat_buffer      _readBuffer;
    std::vector<std::string>       _writeBuffer;
    std::vector<std::string>       _writeQueue;
    std::mutex                     _writeLock;
    std::mutex                     _errorLock; // Both main thread and reader thread can now push error messages.

    boost::asio::thread_pool       _ioc{2};
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _ws{_ioc};
    boost::asio::steady_timer      _timer{_ws.get_executor()};

    std::chrono::milliseconds _tryReconnectTimeout;
    std::chrono::milliseconds _writeDelay = 100ms;
    std::chrono::time_point<std::chrono::steady_clock> _lastWriteTimepoint;

    bool _is_started = false;
    bool _stop_requested = false;
    bool _is_reader_started = false;
    std::promise<void> _readerFinishedPromise;
    std::promise<void> _mainFinishedPromise;
    WebsocketStatus _status{};

    using ErrorCallbackSignature = void(const boost::system::error_code&);
    using MessageCallbackSignature = void(const boost::beast::flat_buffer&);
    using StatusCallbackSignature = void(const WebsocketStatus&);

    nod::signal<ErrorCallbackSignature>   _onErrorSignal;
    nod::signal<MessageCallbackSignature> _onMessageSignal;
    nod::signal<StatusCallbackSignature>  _onStatusSignal;

public:
    WebsocketPayload(const std::string& host, const std::string& port, uint64_t reconnectTimeoutMs);
    ~WebsocketPayload();

    void Start();
    void Stop();

    void Write(const std::string& text);
    
    using ErrorCallbackT = decltype(_onErrorSignal)::slot_type;
    using MessageCallbackT = decltype(_onMessageSignal)::slot_type;
    using StatusCallbackT = decltype(_onStatusSignal)::slot_type;

    nod::connection OnError(ErrorCallbackT&&);
    nod::connection OnMessage(MessageCallbackT&&);
    nod::connection OnStatus(StatusCallbackT&&);

private:
    void do_main_thread();
    void do_reader_thread();
    
    void handle_error(const boost::system::error_code& ec);
    void handle_message(const boost::beast::flat_buffer& data);
    void set_status(const WebsocketStatus& newStatus);

    void do_connect();
    void do_read();
    void do_write();
    void do_reconnect_delay();
    void do_writing_delay();

    void do_stop();
};

} // namespace Network