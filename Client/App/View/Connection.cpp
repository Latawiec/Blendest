#include "public/View/Component/Connection.hpp"

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include <string>
#include <regex>
#include <chrono>

using namespace ftxui;

namespace View {
namespace Component {

namespace /* anonymous */ {
    // These verify the input on the go, but in no way do they verify the final result.
    const std::regex hostInputRegex("^((?:25[0-5]|(?:2[0-4]|1\\d|[1-9]|)\\d)(?:(?:\\.(?:25[0-5]|(?:2[0-4]|1\\d|[1-9]|)\\d)?){0,3}))?");
    const std::regex portInputRegex("^([1-9][0-9]{0,5})?");
}

Connection::Connection()
{
    _hostInput = hostInput();
    _portInput = portInput();
    _connectionButton = connectionButton();
    _connectionData = connectionData();
    _connectionStatus = connectionStatus();

    _component = Container::Vertical({
        _connectionData,
        Renderer([]{ return separator(); }),
        Container::Horizontal({
            Renderer([]{ return filler(); }),
            _connectionStatus | center,
            Renderer([]{ return filler(); }),
            _connectionButton | align_right
        })
    });
}

ftxui::Component Connection::GetComponent()
{
    return Renderer(_component, [&] {
        return window(
            text("Connection") | hcenter | bold,
            _component->Render()
        ) | size(WIDTH, GREATER_THAN, 40);
    });
}

void Connection::SetStatus(Status s)
{
    _status = s;
}

void Connection::SetError(const std::string& error)
{
    _error = error;
}

void Connection::SetHost(const std::string& host)
{
    _host = host;
    _hostInputStr = host;
}

void Connection::SetPort(const std::string& port)
{
    _port = port;
    _portInputStr = port;
}

nod::connection Connection::OnConnect(ConnectCallbackT&& cb)
{
    return _connectSignal.connect(std::move(cb));
}

nod::connection Connection::OnDisconnect(DisconnectCallbackT&& cb)
{
    return _disconnectSignal.connect(std::move(cb));
}

void Connection::connectClick()
{
    _connectSignal(_host, _port);
}

void Connection::disconnectClick()
{
    _disconnectSignal();
}

ftxui::Component Connection::hostInput() {

    InputOption hostInputOptions;
    hostInputOptions.on_change = [&] {
        if (std::regex_match(_hostInputStr, hostInputRegex)) {
            _host = _hostInputStr;
        } else {
            _hostInputStr = _host;
        }
    };

    return Input(&_hostInputStr, "IP Address", hostInputOptions) | size(WIDTH, GREATER_THAN, 20) | flex_grow;
}

ftxui::Component Connection::portInput() {

    InputOption portInputOptions;
    portInputOptions.on_change = [&]{
        if (std::regex_match(_portInputStr, portInputRegex)) {
            _port = _portInputStr;
        } else {
            _portInputStr = _port;
        }
    };

    return Input(&_portInputStr, "Port", portInputOptions) | size(WIDTH, GREATER_THAN, 20) | flex_grow;
}

ftxui::Component Connection::connectionButton() {

    Component connectButton = Button("Connect", [&]{ this->connectClick(); });
    Component disconnectButton = Button("Disconnect", [&]{ this->disconnectClick(); });

    return Container::Horizontal({
        connectButton | Maybe([&]{ return this->_status == Status::DISCONNECTED; }),
        disconnectButton | Maybe([&]{ return this->_status != Status::DISCONNECTED; })
    });
}

ftxui::Component Connection::connectionData()
{
    auto activeInput = Container::Vertical({
        Renderer(_hostInput, [&]{ return hbox({ text("Host: ") | bold, _hostInput->Render() }); }),
        Renderer(_portInput, [&]{ return hbox({ text("Port: ") | bold, _portInput->Render() }); }),
    });

    auto inactiveInput = Container::Vertical({
        Renderer([&]{ return hbox({ text("Host: ") | dim, text(_host) | dim }); }),
        Renderer([&]{ return hbox({ text("Port: ") | dim, text(_port) | dim }); }),
    });

    return Container::Vertical({
        activeInput | Maybe([&]{ return this->_status == Status::DISCONNECTED; }),
        inactiveInput | Maybe([&]{ return this->_status != Status::DISCONNECTED; })
    });
}

ftxui::Component Connection::connectionStatus()
{
    auto connected = Renderer([&]{ return hbox({ text("√"), text(" Connected!") }) | hcenter; });
    auto reconnecting = Renderer([&]{ return hbox({ spinner(4, std::chrono::system_clock::now().time_since_epoch().count() / 5000000), text(" Connecting...") }) | hcenter; });
    
    return Container::Vertical({
        connected | Maybe([&]{ return this->_status == Status::CONNECTED; }),
        reconnecting | Maybe([&]{ return this->_status == Status::RECONNECTING; }),
    });
}

} // namespace Component
} // namespace View