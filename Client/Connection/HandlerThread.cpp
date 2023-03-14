#include "./HandlerThread.hpp"




namespace Connection {

HandlerThread::HandlerThread(const std::string& host, const uint16_t port, const uint64_t loopIntervalMs, const uint64_t tryReconnectTimeMs)
: _host(std::move(host))
, _port(std::to_string(port))
, _loopIntervalDuration(loopIntervalMs)
, _tryReconnectDuration(tryReconnectTimeMs)
, _ioc()
, _resolver(_ioc)
, _websocket(_ioc)
{}

bool HandlerThread::RegisterListener(IListener* listener) const 
{
    std::lock_guard<std::mutex> lock(this->_listenersLock);
    this->_listeners.push_back(listener);
    listener->resetHandler(this);
    return true;
}

bool HandlerThread::UnregisterListener(IListener* listener) const 
{
    std::lock_guard<std::mutex> lock(this->_listenersLock);
    if (auto found = std::remove(_listeners.begin(), _listeners.end(), listener); found != _listeners.end()){
        _listeners.erase(found, _listeners.end());
        return true;
    }
    return false;
}

void HandlerThread::ConnectionDiscarded()
{
    isRunning = false;
    std::lock_guard<std::mutex> lock(this->_listenersLock);
    _listeners.clear();
}

void HandlerThread::Loop() 
{
    while(isRunning) {
        switch (_connectionStatus) {
            case Status::UNINITIALIZED:
            case Status::CLOSED:
                if (!connect()) {
                    break;
                }
            case Status::CONNECTED:
            case Status::READING:
                if (!read()) {
                    _connectionStatus = Status::CLOSED;
                    break;
                }
        }

        if (_connectionStatus == Status::CLOSED) {
            close();
        }

        std::this_thread::sleep_for(_loopIntervalDuration);
    }
}

void HandlerThread::notifyConnect(const boost::system::error_code& errorCode)
{
    Error error{
        .code = errorCode.value(),
        .message = errorCode.what()
    };

    std::lock_guard<std::mutex> lock(this->_listenersLock);
    for(auto& listener : _listeners) {
        listener->OnConnect(error);
    }
}

void HandlerThread::notifyRead(const boost::system::error_code& errorCode, const boost::beast::flat_buffer& buffer)
{
    Error error{
        .code = errorCode.value(),
        .message = errorCode.what()
    };

    Buffer readData;

    if (!error) {
        readData = Buffer{
            .type = 0,
            .data = buffer.data().data(),
            .size = buffer.data().size()
        };
    }

    std::lock_guard<std::mutex> lock(this->_listenersLock);
    for(auto& listener : _listeners) {
        listener->OnRead(error, readData);
    }
}

void HandlerThread::notifyClose(const boost::beast::websocket::close_reason& closeReason)
{
    Error error {
        .code = closeReason.code,
        .message = closeReason.reason.c_str()
    };

    std::lock_guard<std::mutex> lock(this->_listenersLock);
    for(auto& listener : _listeners) {
        listener->OnClose(error);
    }
}


bool HandlerThread::connect()
{
    boost::system::error_code errorCode;
    const auto currentTimePoint = std::chrono::system_clock::now();
    if (_lastConnectionAttempt + _tryReconnectDuration > currentTimePoint) return false;
    // Look up the domain name  
    auto const results = _resolver.resolve(_host, _port, errorCode);

    if (errorCode) {
        notifyConnect(errorCode);
        return false;
    }

    // Make the connection on the IP address we get from a lookup
    auto ep = boost::asio::connect(_websocket.next_layer(), results, errorCode);

    if (errorCode) {
        notifyConnect(errorCode);
        return false;
    }

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    _resolvedAddress = _host + ':' + std::to_string(ep.port());

    // Set a decorator to change the User-Agent of the handshake
    _websocket.set_option(boost::beast::websocket::stream_base::decorator(
        [](boost::beast::websocket::request_type& req)
        {
            req.set(boost::beast::http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                    " websocket-client-coro");
        }));

    // Perform the websocket handshake
    boost::beast::websocket::response_type res;
    _websocket.handshake(res, _resolvedAddress, "/", errorCode);

    if (errorCode) {
        notifyConnect(errorCode);
        return false;
    }

    _connectionStatus = Status::CONNECTED;
    _lastConnectionAttempt = currentTimePoint;
    notifyConnect(errorCode);

    return true;
}

bool HandlerThread::read() {
    boost::system::error_code errorCode;
    _websocket.read(_buffer, errorCode);

    if (errorCode) {
        notifyRead(errorCode, {});
        return false;
    }

    if (_websocket.is_message_done()) {
        _connectionStatus = Status::CONNECTED;
        notifyRead({}, _buffer);
        _buffer.clear();    
    } else {
        _connectionStatus = Status::READING;
    }

    return true;
}

bool HandlerThread::close() {
    notifyClose(_websocket.reason());
    return true;
}


}