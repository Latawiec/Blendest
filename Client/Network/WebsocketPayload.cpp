#include "./WebsocketPayload.hpp"

#include <iostream>

using namespace boost;

namespace Network {

WebsocketPayload::WebsocketPayload(const std::string& host, const std::string& port, uint64_t reconnectTimeoutMs)
: _host(host)
, _port(port)
, _tryReconnectTimeout(std::chrono::milliseconds(reconnectTimeoutMs)) 
{}

WebsocketPayload::~WebsocketPayload(){
    Stop();
}

void WebsocketPayload::Start() {
    asio::post(_ws.get_executor(), [this] {
        _stop_requested = false;
        do_main_thread();
    });
}

void WebsocketPayload::Stop() {
    do_stop();
}

void WebsocketPayload::Write(const std::string& text) {
    std::lock_guard<std::mutex> lock { _writeLock };
    _writeQueue.push_back(text);
}

void WebsocketPayload::OnError(ErrorCallbackT callback) {
    std::lock_guard<std::mutex> lock { _onErrorLock };
    _onError = callback;
}

void WebsocketPayload::OnMessage(MessageCallbackT callback) {
    std::lock_guard lock{ _onMessageLock };
    this->_onMessage = callback;
}

void WebsocketPayload::do_main_thread() {
    while (!_stop_requested) {
        switch(_status) {
            case Status::INIT:         do_connect();         break;
            case Status::CONNECTED:    do_write();           break;
            case Status::DISCONNECTED: do_reconnect_delay(); break;
            case Status::STOPPED:      return;
        };
    }
}

void WebsocketPayload::do_reader_thread() {
    while (!_stop_requested && _status == Status::CONNECTED) {
            do_read();
    }
}

void WebsocketPayload::handle_error(const system::error_code& ec) {
    if (ec.failed()) {
        _status = _stop_requested ? Status::STOPPED : Status::DISCONNECTED;
    }

    if (_stop_requested){
        return;
    }

    {
        std::lock_guard<std::mutex> lock { _onErrorLock };
        _onError(ec);
    }
}

void WebsocketPayload::handle_message(const beast::flat_buffer& data) {
    //if (_stop_requested) return;
    std::lock_guard lock{ _onMessageLock };
    _onMessage(_readBuffer);
    _readBuffer.clear();
}

void WebsocketPayload::do_connect() {
    

    try {
        auto const results = asio::ip::tcp::resolver(_ioc).resolve(_host, _port);
        auto ep = asio::connect(_ws.next_layer(), results);

        _ws.set_option(beast::websocket::stream_base::decorator([](beast::websocket::request_type& req) {
                    req.set(beast::http::field::user_agent,
                            BOOST_BEAST_VERSION_STRING " WsConnect");
                }));
        _ws.handshake(_host + ':' + _port, "/");
        _status = Status::CONNECTED;
        _readBuffer.clear();

    } catch (system::system_error ec) {
        handle_error(ec.code());
        return;
    }

    asio::post(_ws.get_executor(), [this] {
        // Reader thread live.
        do_reader_thread();
    });
}

void WebsocketPayload::do_read() {
    try {
        do {
            _ws.read(_readBuffer);
        } while (!_ws.is_message_done());
        handle_message(_readBuffer);

    } catch (system::system_error ec) {
        handle_error(ec.code());
    }
}

void WebsocketPayload::do_write() {

    do_writing_delay();

    if (_writeBuffer.empty()) {
        std::lock_guard<std::mutex> lock { _writeLock };
        std::swap(_writeBuffer, _writeQueue);
    }

    _lastWriteTimepoint = std::chrono::steady_clock::now();

    // Still empty?
    if (_writeBuffer.empty()) {
        return;
    }

    int messagesPushed = 0;
    try {
        for (const auto& message : _writeBuffer) {
            _ws.write(asio::buffer(message));
        }
        _writeBuffer.clear();
    } catch (system::system_error ec) {
        if (messagesPushed > 0) {
            _writeBuffer.erase(_writeBuffer.begin(), _writeBuffer.begin() + messagesPushed);
        }
        handle_error(ec.code());
    }
}

void WebsocketPayload::do_reconnect_delay() {

    std::this_thread::sleep_for(_tryReconnectTimeout);
    _status = Status::INIT;
}

void WebsocketPayload::do_writing_delay() {

    std::this_thread::sleep_until(_lastWriteTimepoint + _writeDelay);
}

void WebsocketPayload::do_stop() {
    if (!_stop_requested)
        _ws.next_layer().cancel();
}

}