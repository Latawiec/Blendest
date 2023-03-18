#include "./public/Connection.hpp"

namespace Model {

Connection::WebsocketListener::WebsocketListener(UI::Component::Connection& view)
: connectionComponent(view)
{}

Connection::WebsocketListener::~WebsocketListener()
{}


void Connection::WebsocketListener::OnError(const Network::Error& error)
{
    connectionComponent.SetConnectionStatus(UI::Component::Connection::Status::RECONNECTING);
}

void Connection::WebsocketListener::OnMessage(const Network::Buffer& data)
{
    connectionComponent.SetConnectionStatus(UI::Component::Connection::Status::CONNECTED);
}


Connection::Connection(UI::Component::Connection& connectionView)
: _view(connectionView)
, _wsListener(_view)
{
    _view.OnConnectClick = [&](const std::string& host, const std::string& port) {
        wsConnectionOpt.emplace(host, port);
        wsConnectionOpt.value().RegisterListener(&_wsListener);
        wsConnectionOpt.value().Listen();
    };

    _view.OnDisconnectClick = [&](const std::string& host, const std::string& port) {
        wsConnectionOpt.reset();
    };
}

Connection::~Connection() {}

}