#pragma once

#include <cstddef>
#include <string>

namespace Connection
{

    struct Error
    {
        int               code = 0;
        const std::string message;
        operator bool() const
        {
            return code != 0;
        }
    };

    struct Buffer
    {
        uint8_t          type = 0;
        const std::byte* data = nullptr;
        uint64_t         size = 0;
    };

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