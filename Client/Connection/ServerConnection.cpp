#include "./public/ServerConnection.hpp"

#include <thread>
#include <iostream>

Connection::ServerConnection::ServerConnection(std::string host, uint16_t port, uint64_t loopIntervalMs, uint64_t tryReconnectTimeMs)
: _host(std::move(host))
, _port(std::to_string(port))
, _loopIntervalDuration(loopIntervalMs)
, _tryReconnectDuration(tryReconnectTimeMs)
, _ioc()
, _resolver(_ioc)
, _websocket(_ioc)
{}

Connection::ServerConnection::~ServerConnection()
{
    for(auto& listener : _listeners) {
        listener->serverShutdown();
    }
} 

Connection::ServerConnection& Connection::ServerConnection::Start()
{
    this->_asyncThread = std::thread(&ServerConnection::async_main, this);
    this->_asyncThread.detach(); // Just detach it for now.
    return *this;
}

void Connection::ServerConnection::async_main()
{
    while(true) {
        switch (_connectionStatus) {
            case Status::UNINITIALIZED:
            case Status::CLOSED:
                if (!async_connect()) {
                    break;
                }
            case Status::CONNECTED:
            case Status::READING:
                if (!async_read()) {
                    _connectionStatus = Status::CLOSED;
                    break;
                }
        }

        if (_connectionStatus == Status::CLOSED) {
            async_handle_close(_websocket.reason());
        }

        std::this_thread::sleep_for(_loopIntervalDuration);
    }
}

bool Connection::ServerConnection::async_connect() {
    boost::system::error_code errorCode;
    const auto currentTimePoint = std::chrono::system_clock::now();
    if (_lastConnectionAttempt + _tryReconnectDuration > currentTimePoint) return false;
    // Look up the domain name  
    auto const results = _resolver.resolve(_host, _port, errorCode);

    if (errorCode) {
        async_handle_connect(errorCode);
        return false;
    }

    // Make the connection on the IP address we get from a lookup
    auto ep = boost::asio::connect(_websocket.next_layer(), results, errorCode);

    if (errorCode) {
        async_handle_connect(errorCode);
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
        async_handle_connect(errorCode);
        return false;
    }

    _connectionStatus = Status::CONNECTED;
    _lastConnectionAttempt = currentTimePoint;
    async_handle_connect(errorCode);

    return true;
}

bool Connection::ServerConnection::async_read() {
    boost::system::error_code errorCode;
    _websocket.read(_buffer, errorCode);

    if (errorCode) {
        async_handle_read(errorCode, {});
        return false;
    }

    if (_websocket.is_message_done()) {
        _connectionStatus = Status::CONNECTED;
        async_handle_read({}, _buffer);
        _buffer.clear();    
    } else {
        _connectionStatus = Status::READING;
    }

    return true;
}

void Connection::ServerConnection::async_handle_connect(const boost::system::error_code& errorCode) 
{
    std::lock_guard<std::mutex> lock(this->_listenersLock);
    for(auto& listener : _listeners) {
        listener->OnConnect(errorCode);
    }
}

void Connection::ServerConnection::async_handle_read(const boost::system::error_code& errorCode, const boost::beast::flat_buffer& data) 
{
    std::lock_guard<std::mutex> lock(this->_listenersLock);
    for(auto& listener : _listeners) {
        listener->OnRead(errorCode, data);
    }
}

void Connection::ServerConnection::async_handle_close(const boost::beast::websocket::close_reason& reason) 
{
    std::lock_guard<std::mutex> lock(this->_listenersLock);
    for(auto& listener : _listeners) {
        listener->OnClose(reason);
    }
}


bool Connection::ServerConnection::registerListener(IListener* listener) const 
{
    std::lock_guard<std::mutex> lock(this->_listenersLock);
    this->_listeners.push_back(listener);
    return true;
}

bool Connection::ServerConnection::unregisterListener(IListener* listener) const 
{
    std::lock_guard<std::mutex> lock(this->_listenersLock);
    if (auto found = std::remove(_listeners.begin(), _listeners.end(), listener); found != _listeners.end()){
        _listeners.erase(found, _listeners.end());
        return true;
    }
    return false;
}


static uint64_t SingleThreadId = 0;

Connection::ServerConnection::IListener::IListener(const ServerConnection* connection)
: _serverConnection(connection)
, _id(SingleThreadId++)
{
    if (this->_serverConnection != nullptr)  
        this->_serverConnection->registerListener(this);
}

Connection::ServerConnection::IListener::~IListener()
{
    if (this->_serverConnection != nullptr)  
        this->_serverConnection->unregisterListener(this);
}

void Connection::ServerConnection::IListener::serverShutdown()
{
    this->_serverConnection = nullptr;
}

void Connection::ServerConnection::IListener::OnConnect(const boost::system::error_code& error) {}
void Connection::ServerConnection::IListener::OnRead(const boost::system::error_code& error, const boost::beast::flat_buffer& data) {}
void Connection::ServerConnection::IListener::OnClose(const boost::beast::websocket::close_reason& reason) {}