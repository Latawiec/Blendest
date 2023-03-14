#include "./public/ServerConnection.hpp"

#include "HandlerThread.hpp"

#include <thread>
#include <iostream>

namespace Connection {

ServerConnection::ServerConnection(std::string host, uint16_t port, uint64_t loopIntervalMs, uint64_t tryReconnectTimeMs)
: _tmpHandlerThreadPtr(std::make_unique<HandlerThread>(host, port, loopIntervalMs, tryReconnectTimeMs))
, _handlerThreadPtr(_tmpHandlerThreadPtr.get())
{}

ServerConnection::~ServerConnection()
{
    _handlerThreadPtr->ConnectionDiscarded();
} 

ServerConnection& ServerConnection::AddListener(IListener* listener)
{
    _handlerThreadPtr->RegisterListener(listener);
    return *this;
}

ServerConnection& ServerConnection::RemoveListener(IListener* listener)
{
    _handlerThreadPtr->UnregisterListener(listener);
    return *this;
}

ServerConnection& Connection::ServerConnection::Start()
{
    std::thread handler([](std::unique_ptr<HandlerThread> handlerThread){
        handlerThread->Loop();
    }, std::move(_tmpHandlerThreadPtr));

    handler.detach();

    return *this;
}

}