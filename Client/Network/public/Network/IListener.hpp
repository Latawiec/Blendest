#pragma once

#include <cstddef>
#include <string>

#include "Error.hpp"
#include "Buffer.hpp"

namespace Network
{
    class IListener
    {
    public:
        IListener() = default;
        virtual ~IListener() = default;

        virtual void OnError(const Error &error) {};
        virtual void OnMessage(const Buffer &data) {};
        virtual void OnDestroy() {}; // Last chance to move out before the blast.
    };

}