#include "./HandlerPayload.hpp"

#include <iostream>

namespace Connection {

HandlerPayload::HandlerPayload(const std::string& host, const std::string& port, uint64_t reconnectTimeoutMs)
: _host(host)
, _port(port)
, _tryReconnectTimeout(std::chrono::milliseconds(reconnectTimeoutMs)) 
{}

HandlerPayload::~HandlerPayload(){
    Stop();
}

void HandlerPayload::Start() {
    boost::asio::post(_ws.get_executor(), [this] {
        _stop_requested = false;
        do_step_machine();
    });
}

void HandlerPayload::Stop() {
    do_stop();
}

void HandlerPayload::OnError(ErrorCallbackT callback) {
    std::lock_guard<std::mutex> lock { _onErrorLock };
    _onError = callback;
}

void HandlerPayload::OnMessage(MessageCallbackT callback) {
    std::lock_guard lock{ _onMessageLock };
    this->_onMessage = callback;
}

void HandlerPayload::do_step_machine() {
    switch(_status) {
        case Status::INIT:         return do_connect();
        case Status::CONNECTED:    return do_read();
        case Status::DISCONNECTED: return do_reconnect_delay(_tryReconnectTimeout);
        case Status::STOPPED:      break;
    };
}

void HandlerPayload::handle_error(const boost::system::error_code& ec) {
    //if (_stop_requested) return;
    if (ec.failed())
        _status = _stop_requested ? Status::STOPPED : Status::DISCONNECTED;
    {
        std::lock_guard<std::mutex> lock { _onErrorLock };
        _onError(ec);
    }
    do_step_machine();
}

void HandlerPayload::handle_message(const boost::beast::flat_buffer& data) {
    //if (_stop_requested) return;
    std::lock_guard lock{ _onMessageLock };
    _onMessage(_buffer);
    _buffer.clear();
    do_step_machine();
}

void HandlerPayload::do_connect() {
    using tcp = boost::asio::ip::tcp;
    using error_code = boost::system::error_code;
    using namespace boost;

    async_connect(
        _ws.next_layer(), tcp::resolver(_ioc).resolve(_host, _port),
        [this](error_code ec, tcp::endpoint /*ep*/) {
            if (ec) return handle_error(ec);

            _ws.set_option(beast::websocket::stream_base::decorator([](beast::websocket::request_type& req) {
                req.set(beast::http::field::user_agent,
                        BOOST_BEAST_VERSION_STRING " WsConnect");
            }));

            _ws.async_handshake(_host + ':' + _port, "/", [this](error_code ec) {
                if (ec) return handle_error(ec);
                _status = Status::CONNECTED;
                _buffer.clear();
                do_step_machine();
            });
        }
    );
}

void HandlerPayload::do_read() {
    using error_code = boost::system::error_code;
    using namespace boost;
    _ws.async_read(_buffer, [this](error_code ec, size_t) {
        if (ec) return handle_error(ec);

        if (_ws.is_message_done()) {
            return handle_message(_buffer);
        } else {
            do_read();
        }
    });
}

void HandlerPayload::do_reconnect_delay(std::chrono::steady_clock::duration d) {
    using error_code = boost::system::error_code;
    _timer.expires_after(d);
    _timer.async_wait([this](error_code ec) {
        if (ec) return handle_error(ec);
        _status = Status::INIT;
        do_step_machine();
    });
}

std::future<void> HandlerPayload::do_stop() {
    using namespace boost;
    return dispatch(_ws.get_executor(), std::packaged_task<void()>([this] {
                        _stop_requested = true;
                        _ws.next_layer().cancel();
                        _ws.close(beast::websocket::normal);
                    }));
}

}