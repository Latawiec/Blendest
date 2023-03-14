#pragma once

#include <cstddef>
#include <string>

namespace Connection {

class HandlerThread;

struct Error {
    int          code = 0;
    const std::string message;

    operator bool() const {
        return code != 0;
    }
};

struct Buffer {
    uint8_t     type = 0;
    const void* data = nullptr;
    uint64_t    size = 0;
};

class IListener {
    friend class HandlerThread;

    using ID = uint64_t;
    const ID             _id;
    const HandlerThread* _handler = nullptr;

    void resetHandler(const HandlerThread*);

public:
    IListener(const IListener&) = delete;
    IListener& operator=(const IListener&) = delete;


    IListener();
    virtual ~IListener();
    virtual void OnConnect(const Error& error);
    virtual void OnRead(const Error& error, const Buffer& data);
    virtual void OnClose(const Error& reason);
};

}