#include "public/Model/Connection.hpp"

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
        _data.hostAddress = host;
        _data.hostPort = port;
    };

    _view.OnDisconnectClick = [&](const std::string& host, const std::string& port) {
        wsConnectionOpt.reset();
    };
}

Connection::~Connection() {}

const Connection::Data& Connection::getData() const
{
    return this->_data;
}

void Connection::setData(Connection::Data newData)
{
    _view.setHostAddress(newData.hostAddress);
    _view.setHostPort(newData.hostPort);
    this->_data = std::move(newData);
}

}