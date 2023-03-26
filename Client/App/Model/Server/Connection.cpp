#include "public/Connection.hpp"

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
    _http.Start();
}

void Connection::Stop()
{
    _websocket.Stop();
    _http.Stop();
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