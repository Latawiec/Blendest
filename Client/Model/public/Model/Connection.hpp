#pragma once 

#include <UI/Component/Connection.hpp>
#include <Network/BackgroundWebsocket.hpp>
#include <Network/HttpSession.hpp>
#include <Network/IListener.hpp>


namespace Model {

class Connection {
public:
    struct Data {
        std::string hostAddress;
        std::string hostPort;
    };

private:
    UI::Component::Connection& _view;
    std::optional<Network::BackgroundWebsocket> wsConnectionOpt = std::nullopt;
    std::optional<Network::HttpSession> httpSession = std::nullopt;

    class WebsocketListener : public Network::IListener {
        UI::Component::Connection& connectionComponent;
    public:
        WebsocketListener(UI::Component::Connection& view);
        virtual ~WebsocketListener();

        void OnError(const Network::Error& error) override;
        void OnMessage(const Network::Buffer& data) override;

    } _wsListener;

    Data _data;

public:
    Connection(UI::Component::Connection& connectionView);
    ~Connection();

    const Data& getData() const;
    void setData(Data newData);
};

}