#include "./public/Connection.hpp"

#include "ftxui/dom/elements.hpp"

#include <string>
#include <regex>
#include <chrono>

using namespace ftxui;

namespace UI {

namespace /* anonymous */ {
    const std::regex hostInputRegex("^((?:25[0-5]|(?:2[0-4]|1\\d|[1-9]|)\\d)(?:(?:\\.(?:25[0-5]|(?:2[0-4]|1\\d|[1-9]|)\\d)?){0,3}))?");
    const std::regex portInputRegex("^([1-9][0-9]{0,5})?");
}

Connection::Connection()
{
    InputOption hostInputOptions;
    hostInputOptions.on_change = [&]{
        if (std::regex_match(_hostInputStr, hostInputRegex)) {
            _host = _hostInputStr;
        } else {
            _hostInputStr = _host;
        }

    };

    InputOption portInputOptions;
    portInputOptions.on_change = [&]{
        if (std::regex_match(_portInputStr, portInputRegex)) {
            _port = _portInputStr;
        } else {
            _portInputStr = _port;
        }
    };
    

    _hostInput = Input(&_hostInputStr, "IP Address", hostInputOptions) | size(WIDTH, GREATER_THAN, 20) | flex_grow;
    _portInput = Input(&_portInputStr, "Socket", portInputOptions) | size(WIDTH, GREATER_THAN, 20) | flex_grow;
    _connectionButton = Container::Vertical({
        Button("Connect", [&]{ this->OnConnect(_host, _port); _showMonitoring = true; _showInitialize = false; }) | Maybe(&_showInitialize),
        Button("Disconnect", [&]{ this->OnDisconnect(_host, _port); _showMonitoring = false; _showInitialize = true; }) | Maybe(&_showMonitoring),
    }) | align_right;

    _connectionInput = Container::Vertical({
      Renderer(_hostInput, [&]{ return hbox({ text("Host: ") | bold, _hostInput->Render() }); }),
      Renderer(_portInput, [&]{ return hbox({ text("Port: ") | bold, _portInput->Render() }); }),
    });

    _connectionDisplay = Container::Vertical({
        Renderer([&]{ return hbox({ text("Host: ") | dim, text(_host) | dim }); }),
        Renderer([&]{ return hbox({ text("Port: ") | dim, text(_port) | dim }); }),
    });



    _connectionStatus = Container::Vertical({
        Renderer([&]{
            return hbox ({
                spinner(4, std::chrono::system_clock::now().time_since_epoch().count() / 5000000), text(" Connecting...")
            }) | hcenter;
        }) | Maybe(&_showReconnecting),
        Renderer([&]{
            return hbox({
                text("√"), text(" Connected!")
            }) | hcenter;
        }) | Maybe(&_showConnected),
    });


    _component = Container::Vertical({
        _connectionInput | Maybe(&_showInitialize),
        _connectionDisplay | Maybe(&_showMonitoring),
        Renderer([]{ return separator(); }),
        Container::Horizontal({
            Renderer([]{ return filler(); }),
            _connectionStatus | Maybe(&_showMonitoring) | center,
            Renderer([]{ return filler(); }),
            _connectionButton | align_right
        }),
    });
}

ftxui::Component Connection::getComponent()
{
    return Renderer(_component, [&] {
        return window(
            text("Connection") | hcenter | bold,
            _component->Render()
        ) | size(WIDTH, GREATER_THAN, 40);
    });
}

void Connection::SetConnectionStatus(Status status) 
{
    switch(status){
        case Connection::Status::CONNECTED:
        _showReconnecting = false;
        _showConnected = true;
        break;
        case Connection::Status::RECONNECTING:
        _showReconnecting = true;
        _showConnected = false;
        break;
    }
}

}