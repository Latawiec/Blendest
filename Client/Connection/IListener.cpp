#include "./public/IListener.hpp"

#include "HandlerThread.hpp"

namespace Connection {

static uint64_t SingleThreadId = 0;

IListener::IListener()
: _id(SingleThreadId++)
{}

IListener::~IListener()
{
    if (this->_handler != nullptr)  
        this->_handler->UnregisterListener(this);
}

void IListener::resetHandler(const HandlerThread* handler)
{
    if (this->_handler != nullptr) {
        this->_handler->UnregisterListener(this);
    }

    this->_handler = handler;
}

void IListener::OnConnect(const Error& error) {}
void IListener::OnRead(const Error& error, const Buffer& data) {}
void IListener::OnClose(const Error& reason) {}

}