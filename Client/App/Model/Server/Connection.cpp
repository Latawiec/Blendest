#include "public/Model/Server/Connection.hpp"

namespace Model {
namespace Server {

Connection::Connection(std::string host, std::string port)
: _hostAddress(std::move(host))
, _hostPort(std::move(port))
, _websocket(_hostAddress, _hostPort)
, _http(_hostAddress, _hostPort)
{}

Connection::~Connection()
{}

void Connection::Reset(std::string host, std::string port)
{
    _hostAddress = std::move(host);
    _hostPort = std::move(port);
    _websocket = Network::Websocket(_hostAddress, _hostPort);
    _http = Network::HttpSession(_hostAddress, _hostPort);
}

void Connection::Start()
{
    _websocket.Start();
}

void Connection::Stop()
{
    _websocket.Stop();
}

const std::string& Connection::GetHost() const
{
    return _hostAddress;
}

const std::string& Connection::GetPort() const
{
    return _hostPort;   
}

Network::Websocket& Connection::Websocket()
{
    return _websocket;
}

Network::HttpSession& Connection::Http()
{
    return _http;
}


} // namespace Server
} // namespace Model