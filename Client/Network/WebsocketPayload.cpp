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
    // We need to make sure everything is done, otherwise we might sweep resources from a running thread.
    _mainFinishedPromise.get_future().get();
}

void WebsocketPayload::Write(const std::string& text) {
    std::lock_guard<std::mutex> lock { _writeLock };
    _writeQueue.push_back(text);
}

nod::connection WebsocketPayload::OnError(ErrorCallbackT&& cb) {
    return _onErrorSignal.connect(std::move(cb));
}
nod::connection WebsocketPayload::OnMessage(MessageCallbackT&& cb) {
    return _onMessageSignal.connect(std::move(cb));
}

nod::connection WebsocketPayload::OnStatus(StatusCallbackT&& cb) {
    return _onStatusSignal.connect(std::move(cb));
}

void WebsocketPayload::do_main_thread() {
    _mainFinishedPromise = std::promise<void>{};
    while (!_stop_requested) {
        switch(_status) {
            case WebsocketStatus::INIT:         do_connect();         break;
            case WebsocketStatus::CONNECTED:    do_write();           break;
            case WebsocketStatus::DISCONNECTED: do_reconnect_delay(); break;
            case WebsocketStatus::STOPPED:      break;
        };
    }

    if (_is_reader_started) {
        _readerFinishedPromise.get_future().get();
        _is_reader_started = false;
    }
    _mainFinishedPromise.set_value();
}

void WebsocketPayload::do_reader_thread() {
    while (!_stop_requested && _status == WebsocketStatus::CONNECTED) {
            do_read();
    }
}

void WebsocketPayload::handle_error(const system::error_code& ec) {
    std::lock_guard<std::mutex> lock { _errorLock };
    if (ec.failed()) {
        set_status(_stop_requested ? WebsocketStatus::STOPPED : WebsocketStatus::DISCONNECTED);
    }

    if (_stop_requested){
        return;
    }
    _onErrorSignal(ec);
}

void WebsocketPayload::handle_message(const beast::flat_buffer& data) {
    _onMessageSignal(data);
    _readBuffer.clear();
}

void WebsocketPayload::set_status(const WebsocketStatus& status) {
    _status = status;
    _onStatusSignal(status);
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
        set_status(WebsocketStatus::CONNECTED);
        _readBuffer.clear();

    } catch (system::system_error ec) {
        handle_error(ec.code());
        return;
    }

    _is_reader_started = true;
    asio::post(_ws.get_executor(), [this] {
        _readerFinishedPromise = std::promise<void>{};
        // Reader thread live.
        do_reader_thread();
        _readerFinishedPromise.set_value();
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
    const auto entryTimestamp = std::chrono::steady_clock::now();

    // Wait for reader to close up. We don't want to accidentaly post more than 1 reader.
    if (_is_reader_started) {
        _readerFinishedPromise.get_future().get();
        _is_reader_started = false;
    }

    std::this_thread::sleep_until(entryTimestamp + _tryReconnectTimeout);
    set_status(WebsocketStatus::INIT);
}

void WebsocketPayload::do_writing_delay() {

    std::this_thread::sleep_until(_lastWriteTimepoint + _writeDelay);
}

void WebsocketPayload::do_stop() {
    if (!_stop_requested) {
        _stop_requested = true;
        _ws.next_layer().cancel();
    }
}

}