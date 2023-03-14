#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "./public/IListener.hpp"

namespace Connection {

class HandlerThread {
    friend class IListener;

    mutable std::mutex _listenersLock;
    mutable std::vector<IListener*> _listeners;

    enum class Status {
        UNINITIALIZED,
        CLOSED,
        CONNECTED,
        READING,
    };

    const std::string _host;
    const std::string _port;
    std::string _resolvedAddress;

    boost::asio::io_context        _ioc;
    boost::asio::ip::tcp::resolver _resolver;
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _websocket;

    boost::beast::flat_buffer _buffer;

    std::chrono::milliseconds _loopIntervalDuration;
    std::chrono::milliseconds _tryReconnectDuration;
    std::chrono::time_point<std::chrono::system_clock> _lastConnectionAttempt;

    bool isRunning = true;
    Status _connectionStatus = Status::UNINITIALIZED;

public:
    HandlerThread(const std::string& host, const uint16_t port, const uint64_t loopIntervalMs, const uint64_t tryReconnectTimeMs);
    ~HandlerThread() = default;

    void Loop();

    bool RegisterListener(IListener* listener) const;
    bool UnregisterListener(IListener* listener) const;

    void ConnectionDiscarded();

private:
    bool connect();
    bool read();
    bool close();

    void notifyConnect(const boost::system::error_code& errorCode);
    void notifyRead(const boost::system::error_code& errorCode, const boost::beast::flat_buffer& data);
    void notifyClose(const boost::beast::websocket::close_reason& errorCode);
};

} // namespace Connection