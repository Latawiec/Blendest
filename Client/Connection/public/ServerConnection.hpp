#pragma once

#include <string>
#include <chrono>
#include <thread>

#include "IListener.hpp"

namespace Connection {

class HandlerThread;

class ServerConnection {
    std::unique_ptr<HandlerThread> _tmpHandlerThreadPtr;
    HandlerThread* _handlerThreadPtr;
public:
    ServerConnection(std::string host, uint16_t port, uint64_t loopIntervalMs = 50, uint64_t tryReconnectTimeMs = 500);
    ~ServerConnection();

    ServerConnection& AddListener(IListener* listener);
    ServerConnection& RemoveListener(IListener* listener);

    ServerConnection& Start();
}; 

} // namespace Connection
