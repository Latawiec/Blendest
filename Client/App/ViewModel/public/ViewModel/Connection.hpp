#pragma once

#include <Model/Server/Connection.hpp>
#include <View/Component/Connection.hpp>

#include <nod.hpp>

namespace ViewModel {

class Connection {

    Model::Server::Connection& _model;
    View::Component::Connection& _view;

    nod::connection _onConnectHandle;
    nod::connection _onDisconnectHandle;

    struct {
        nod::connection onStatusHandle;
    } _websocket;

public:
    Connection(Model::Server::Connection& model, View::Component::Connection& view);

private:
    void onConnect(const std::string& host, const std::string& port);
    void onDisconnect();
};

} // namespace ViewModel