#include "public/Network/HttpSession.hpp"

#include "HttpSessionPayload.hpp"

namespace Network {

HttpSession::HttpSession(const std::string& host, const std::string& port)
: _payloadHandle{ std::make_unique<HttpSessionPayload>(host, port) }
{
    
}

HttpSession::~HttpSession()
{
    _payloadHandle->Stop();
}

HttpSession::HttpSession(HttpSession&& other)
{
    std::swap(_payloadHandle, other._payloadHandle);
}

HttpSession& HttpSession::operator=(HttpSession&& other)
{
    std::swap(_payloadHandle, other._payloadHandle);
    return *this;
}

void HttpSession::Start() 
{
    _payloadHandle->Start();
}

void HttpSession::Stop()
{
    _payloadHandle->Stop();
}

std::future<Error> HttpSession::GetFile(const std::string& target, const std::string& outputFilePath)
{
    return _payloadHandle->GetFile(target, outputFilePath);
}

std::future<Error> HttpSession::SendFile(const std::string& target, const std::string& inputFilePath)
{
    return _payloadHandle->SendFile(target, inputFilePath);
}

}