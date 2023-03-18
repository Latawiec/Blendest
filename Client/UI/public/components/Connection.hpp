#pragma once

#include <string>
#include <memory>
#include <functional>

namespace ftxui {
class ComponentBase;
}

namespace UI {
namespace Component {

class Connection {
    using Component = std::shared_ptr<ftxui::ComponentBase>;

    std::string _host = "";
    std::string _port = "";

    std::string _hostInputStr;
    Component _hostInput;
    std::string _portInputStr;
    Component _portInput;

    Component _connectionInput;
    Component _connectionDisplay;
    Component _connectionStatus;

    Component _connectionButton;

    std::string _errorMessage;

    bool _showInitialize = true;
    bool _showMonitoring = false;

    bool _showReconnecting = true;
    bool _showConnected = false;
    Component _component;

public:
    Connection();
    Component getComponent();

    using OnConnectFunctionT = std::function<void(const std::string&, const std::string&)>;
    using OnDisconnectFunctionT = std::function<void(const std::string&, const std::string&)>;

    OnConnectFunctionT OnConnectClick;
    OnDisconnectFunctionT OnDisconnectClick;

    enum Status {
        RECONNECTING,
        CONNECTED
    };

    void SetConnectionStatus(Status s);
};

}
}