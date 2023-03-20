#include "./public/Network/BackgroundWebsocket.hpp"

#include <boost/asio/use_future.hpp>
#include <optional>
#include <memory>

#include "WebsocketPayload.hpp"
#include "public/Network/IListener.hpp"

namespace Network {

BackgroundWebsocket::BackgroundWebsocket(const std::string& host, const std::string& port, uint64_t reconnectTimeoutMs)
: _payloadHandle{ std::make_unique<WebsocketPayload>(host, port, reconnectTimeoutMs) }
{
    _payloadHandle->OnMessage([&](const boost::beast::flat_buffer& buffer){
        std::lock_guard<std::mutex> lock{ _listenersLock };
        for(auto& listener : _listeners) {
            listener->OnMessage({
                .type = 0,
                .data = static_cast<const std::byte*>(buffer.data().data()),
                .size = buffer.size()
            });
        }
    });

    _payloadHandle->OnError([&](const boost::system::error_code& error){
        std::lock_guard<std::mutex> lock{ _listenersLock };
        for(auto& listener : _listeners) {
            listener->OnError({
                .code = error.value(),
                .message = error.what()
            });
        }
    });
}

BackgroundWebsocket::~BackgroundWebsocket() 
{
    _payloadHandle->OnError([](const auto& e){});
    _payloadHandle->OnMessage([](const auto& e){});
    _payloadHandle->Stop();

    for(auto& listener : _listeners) {
        listener->OnDestroy();
    }
}

void BackgroundWebsocket::Listen() {
    _payloadHandle->Start();
}

void BackgroundWebsocket::RegisterListener(IListener* listener)
{
    std::lock_guard<std::mutex> lock{ _listenersLock };
    _listeners.push_back(listener);
};

void BackgroundWebsocket::UnregisterListener(IListener* listener)
{
    std::lock_guard<std::mutex> lock{ _listenersLock };
    _listeners.erase(std::remove(_listeners.begin(), _listeners.end(), listener), _listeners.end());
};

}