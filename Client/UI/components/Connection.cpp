#include "./public/Connection.hpp"

#include "ftxui/dom/elements.hpp"

#include <string>
#include <chrono>

using namespace ftxui;

namespace UI {

Connection::Connection()
{
    _hostInput = Input(&_host, "IP Address") | size(WIDTH, GREATER_THAN, 20) | flex_grow;
    _portInput = Input(&_port, "Socket") | size(WIDTH, GREATER_THAN, 20) | flex_grow;
    _connectionButton = Container::Vertical({
        Button("Connect", [&]{ _showMonitoring = true; _showInitialize = false; }) | Maybe(&_showInitialize),
        Button("Disconnect", [&]{ _showMonitoring = false; _showInitialize = true; }) | Maybe(&_showMonitoring),
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

}