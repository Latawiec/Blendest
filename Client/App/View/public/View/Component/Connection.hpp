#pragma once

#include <string>
#include <memory>
#include <functional>

#include <nod.hpp>

namespace ftxui {
class ComponentBase;
}

namespace View {
namespace Component {

class Connection {
public:
    enum Status {
        DISCONNECTED,
        RECONNECTING,
        CONNECTED
    };

private:
    using Component = std::shared_ptr<ftxui::ComponentBase>;

    std::string _host = "";
    std::string _port = "";
    std::string _error = "";
    Status _status = Status::DISCONNECTED;

    std::string _hostInputStr;
    std::string _portInputStr;

    Component _hostInput;
    Component _portInput;
    Component _connectionButton;

    Component _connectionData;
    Component _connectionStatus;

    Component _component;

    using ConnectCallbackSignature = void(const std::string&, const std::string&);
    using DisconnectCallbackSignature = void();

    nod::signal<ConnectCallbackSignature> _connectSignal;
    nod::signal<DisconnectCallbackSignature> _disconnectSignal;

public:
    Connection();
    Component GetComponent();

    void SetStatus(Status s);
    void SetError(const std::string& error);
    void SetHost(const std::string& host);
    void SetPort(const std::string& port);

    using ConnectCallbackT = decltype(_connectSignal)::slot_type;
    using DisconnectCallbackT = decltype(_disconnectSignal)::slot_type;

    [[nodiscard]] nod::connection OnConnect(ConnectCallbackT&&);
    [[nodiscard]] nod::connection OnDisconnect(DisconnectCallbackT&&);

private:
    void connectClick();
    void disconnectClick();

    Component hostInput();
    Component portInput();
    Component connectionButton();
    Component connectionData();
    Component connectionStatus();
};

} // namespace Component
} // namespace View