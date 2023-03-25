#pragma once 

#include <UI/Component/Connection.hpp>
#include <Network/Websocket.hpp>
#include <Network/HttpSession.hpp>

#include <nod.hpp>

namespace Model {

class Connection {
public:
    struct Data {
        std::string hostAddress;
        std::string hostPort;
    };

private:
    UI::Component::Connection& _view;
    std::optional<Network::Websocket> wsConnectionOpt = std::nullopt;
    std::optional<Network::HttpSession> httpSession = std::nullopt;

    nod::connection _onErrorConnection;
    nod::connection _onMessageConnection;
    nod::connection _onStatusConnection;


    Data _data;

public:
    Connection(UI::Component::Connection& connectionView);
    ~Connection();

    const Data& getData() const;
    void setData(Data newData);
};

}