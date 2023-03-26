#pragma once

#include "WebsocketStatus.hpp"
#include "Error.hpp"
#include "Buffer.hpp"

#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <functional>
#include <algorithm>

#include <nod.hpp>

namespace Network
{
    class WebsocketPayload;

    class Websocket
    {
    private:
        std::unique_ptr<WebsocketPayload> _payloadHandle;

    public:
        Websocket(const std::string &host, const std::string &port, uint64_t reconnectTimeoutMs = 1000);
        ~Websocket();

        void Listen();
        void Write(const std::string& text);

        using OnErrorCallbackT   = std::function<void(const Error&)>;
        using OnMessageCallbackT = std::function<void(const Buffer&)>;
        using OnStatusCallbackT  = std::function<void(const WebsocketStatus&)>;

        [[nodiscard]] nod::connection OnError(OnErrorCallbackT&&);
        [[nodiscard]] nod::connection OnMessage(OnMessageCallbackT&&);
        [[nodiscard]] nod::connection OnStatus(OnStatusCallbackT&&);
    };

} // namespace Network