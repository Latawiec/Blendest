#pragma once

#include "Buffer.hpp"
#include "Error.hpp"

#include <string>
#include <memory>
#include <future>


namespace Network {

class HttpSessionPayload;

class HttpSession {

    std::unique_ptr<HttpSessionPayload> _payloadHandle;

public:
    HttpSession(const std::string& host, const std::string& port);
    ~HttpSession();

    HttpSession(HttpSession&&);
    HttpSession& operator=(HttpSession&&);

    void Start();
    void Stop();

    std::future<Error> GetFile(const std::string& target, const std::string& outputFilePath);
};

}