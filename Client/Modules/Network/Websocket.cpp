#include "./public/Network/Websocket.hpp"

#include <boost/asio/use_future.hpp>
#include <optional>
#include <memory>

#include "WebsocketPayload.hpp"

namespace Network {

Websocket::Websocket(const std::string& host, const std::string& port, uint64_t reconnectTimeoutMs)
: _payloadHandle{ std::make_unique<WebsocketPayload>(host, port, reconnectTimeoutMs) }
{}

Websocket::~Websocket() 
{}

Websocket::Websocket(Websocket&& other) 
{
    std::swap(_payloadHandle, other._payloadHandle);
}

Websocket& Websocket::operator=(Websocket&& other) 
{
    std::swap(_payloadHandle, other._payloadHandle);
    return *this;
}


void Websocket::Start() {
    _payloadHandle->Start();
}

void Websocket::Stop() {
    _payloadHandle->Stop();
}

void Websocket::Write(const std::string& text) {
    _payloadHandle->Write(text);
}

nod::connection Websocket::OnError(OnErrorCallbackT&& cb) {
    return _payloadHandle->OnError([cb = std::move(cb)] (const boost::system::error_code& error_code) {
        Error e = {
            .code = error_code.value(),
            .message = error_code.message()
        };
        cb(e);
    });
}

nod::connection Websocket::OnMessage(OnMessageCallbackT&& cb) {
    return _payloadHandle->OnMessage([cb = std::move(cb)] (const boost::beast::flat_buffer& buffer) {
        Buffer m = {
            .type = 0, 
            .data = reinterpret_cast<const std::byte*>(buffer.data().data()),
            .size = buffer.data().size()
        };
        cb(m);
    });
}

nod::connection Websocket::OnStatus(OnStatusCallbackT&& cb) {
    return _payloadHandle->OnStatus(std::move(cb));
}


}