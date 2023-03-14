#pragma once
#include "ftxui/component/component.hpp"  // for Slider, Checkbox, Vertical, Renderer, Button, Input, Menu, Radiobox, Toggle

namespace UI {

class Connection {
    
    std::string _host = "";
    std::string _port = "";

    ftxui::Component _hostInput;
    ftxui::Component _portInput;

    ftxui::Component _connectionInput;
    ftxui::Component _connectionDisplay;
    ftxui::Component _connectionStatus;

    ftxui::Component _connectionButton;

    std::string _errorMessage;

    bool _showInitialize = true;
    bool _showMonitoring = false;

    bool _showReconnecting = false;
    bool _showConnected = true;
    ftxui::Component _component;

public:
    Connection();
    ftxui::Component getComponent();
};

}