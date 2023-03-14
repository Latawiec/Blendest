#pragma once

#include <string>
#include <chrono>
#include <thread>

#include "IListener.hpp"

namespace Connection {

class HandlerThread;

class WebsocketConnection {
    std::unique_ptr<HandlerThread> _tmpHandlerThreadPtr;
    HandlerThread* _handlerThreadPtr;
public:
    WebsocketConnection(std::string host, uint16_t port, uint64_t loopIntervalMs = 50, uint64_t tryReconnectTimeMs = 500);
    ~WebsocketConnection();

    WebsocketConnection& AddListener(IListener* listener);
    WebsocketConnection& RemoveListener(IListener* listener);

    WebsocketConnection& Start();
}; 

} // namespace Connection
