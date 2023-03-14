#include "./public/WebsocketConnection.hpp"

#include "HandlerThread.hpp"

#include <thread>
#include <iostream>

namespace Connection {

WebsocketConnection::WebsocketConnection(std::string host, uint16_t port, uint64_t loopIntervalMs, uint64_t tryReconnectTimeMs)
: _tmpHandlerThreadPtr(std::make_unique<HandlerThread>(host, port, loopIntervalMs, tryReconnectTimeMs))
, _handlerThreadPtr(_tmpHandlerThreadPtr.get())
{}

WebsocketConnection::~WebsocketConnection()
{
    _handlerThreadPtr->ConnectionDiscarded();
} 

WebsocketConnection& WebsocketConnection::AddListener(IListener* listener)
{
    _handlerThreadPtr->RegisterListener(listener);
    return *this;
}

WebsocketConnection& WebsocketConnection::RemoveListener(IListener* listener)
{
    _handlerThreadPtr->UnregisterListener(listener);
    return *this;
}

WebsocketConnection& Connection::WebsocketConnection::Start()
{
    std::thread handler([](std::unique_ptr<HandlerThread> handlerThread){
        handlerThread->Loop();
    }, std::move(_tmpHandlerThreadPtr));

    handler.detach();

    return *this;
}

}