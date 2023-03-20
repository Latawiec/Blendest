#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

namespace Network
{
    class WebsocketPayload;
    class IListener;

    class BackgroundWebsocket
    {
    private:
        std::unique_ptr<WebsocketPayload> _payloadHandle;

        std::mutex _listenersLock;
        std::vector<IListener*> _listeners;

    public:
        BackgroundWebsocket(const std::string &host, const std::string &port, uint64_t reconnectTimeoutMs = 1000);
        ~BackgroundWebsocket();

        void Listen();

        void RegisterListener(IListener*listener);
        void UnregisterListener(IListener*listener);
    };

} // namespace Network