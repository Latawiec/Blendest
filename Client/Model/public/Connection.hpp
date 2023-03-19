#pragma once 

#include <components/Connection.hpp>
#include <BackgroundWebsocket.hpp>
#include <IListener.hpp>


namespace Model {

class Connection {
    UI::Component::Connection& _view;
    std::optional<Network::BackgroundWebsocket> wsConnectionOpt = std::nullopt;
    class WebsocketListener : public Network::IListener {
        UI::Component::Connection& connectionComponent;
    public:
        WebsocketListener(UI::Component::Connection& view);
        virtual ~WebsocketListener();

        void OnError(const Network::Error& error) override;
        void OnMessage(const Network::Buffer& data) override;

    } _wsListener;


public:
    Connection(UI::Component::Connection& connectionView);
    ~Connection();

};

}