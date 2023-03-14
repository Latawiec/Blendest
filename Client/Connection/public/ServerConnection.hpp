#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>
#include <chrono>
#include <thread>

namespace Connection {

class ServerConnection {

public:
    enum class Status {
        UNINITIALIZED,
        CLOSED,
        CONNECTED,
        READING,
    };

    class IListener {
        friend class ServerConnection;

        using ID = uint64_t;
        const ServerConnection* _serverConnection;
        const ID                _id;

        void serverShutdown();
    public:
        IListener() = delete;
        IListener(const IListener&) = delete;
        IListener& operator=(const IListener&) = delete;


        IListener(const ServerConnection*);
        virtual ~IListener();
        virtual void OnConnect(const boost::system::error_code& error);
        virtual void OnRead(const boost::system::error_code& error, const boost::beast::flat_buffer& data);
        virtual void OnClose(const boost::beast::websocket::close_reason& reason);
    };

private:
    const std::string _host;
    const std::string _port;
    std::string _resolvedAddress;

    boost::asio::io_context        _ioc;
    boost::asio::ip::tcp::resolver _resolver;
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _websocket;

    boost::beast::flat_buffer _buffer;

    Status _connectionStatus = Status::UNINITIALIZED;

    std::thread _asyncThread;
    std::chrono::milliseconds _loopIntervalDuration;
    std::chrono::milliseconds _tryReconnectDuration;
    std::chrono::time_point<std::chrono::system_clock> _lastConnectionAttempt;

public:
    ServerConnection(std::string host, uint16_t port, uint64_t loopIntervalMs = 50, uint64_t tryReconnectTimeMs = 500);
    ~ServerConnection();

    ServerConnection& Start();

private:
// Thread stuff.
    void async_main();

    bool async_connect();
    bool async_read();
    bool async_close();

    void async_handle_connect(const boost::system::error_code& errorCode);
    void async_handle_read(const boost::system::error_code& errorCode, const boost::beast::flat_buffer& data);
    void async_handle_close(const boost::beast::websocket::close_reason& reason);

// Only listeners care about these, nobody else.
    bool registerListener(IListener* listener) const;
    bool unregisterListener(IListener* listener) const;
    mutable std::mutex _listenersLock;
    mutable std::vector<IListener*> _listeners;
}; 

} // namespace Connection
