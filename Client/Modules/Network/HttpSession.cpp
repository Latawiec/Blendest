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

void HttpSession::Start() 
{
    _payloadHandle->Start();
}

std::future<Error> HttpSession::GetFile(const std::string& target, const std::string& outputFilePath)
{
    return _payloadHandle->GetFile(target, outputFilePath);
}


}