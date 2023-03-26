#include "public/ViewModel/Connection.hpp"

namespace ViewModel {

Connection::Connection(Model::Server::Connection& model, View::Component::Connection& view)
: _model(model)
, _view(view)
{
    _view.SetStatus(View::Component::Connection::Status::DISCONNECTED);
    _view.SetError("");
    _view.SetHost(_model.GetHost());
    _view.SetPort(_model.GetPort());

    _onConnectHandle = _view.OnConnect([this](const std::string& host, const std::string& port){ onConnect(host, port); });
    _onDisconnectHandle = _view.OnDisconnect([this]{ onDisconnect(); });
}

void Connection::onConnect(const std::string& host, const std::string& port)
{
    // TODO: Validate input.
    // if (input bad) { _view.SetError("Bad bad"); return; }

    _model.Reset(host, port);
    _websocket.onStatusHandle = _model.Websocket().OnStatus([this] (const Network::WebsocketStatus& status) {
        if (status == Network::WebsocketStatus::CONNECTED) {
            _view.SetStatus(View::Component::Connection::Status::CONNECTED);
        } else {
            _view.SetStatus(View::Component::Connection::Status::RECONNECTING);
        }
    });
    _model.Start();
}

void Connection::onDisconnect()
{
    _websocket.onStatusHandle = {};
    _model.Stop();
    _view.SetStatus(View::Component::Connection::Status::DISCONNECTED);
    _view.SetError("");
}

} // namespace ViewModel