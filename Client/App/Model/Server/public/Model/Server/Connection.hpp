#pragma once

#include <Network/Websocket.hpp>
#include <Network/HttpSession.hpp>

#include <string>

namespace Model {
namespace Server {

class Connection {

    std::string _hostAddress;
    std::string _hostPort;

    Network::Websocket _websocket;
    Network::HttpSession _http;

public:
    Connection(std::string address = "", std::string port = "");
    ~Connection();

    void Reset(std::string address, std::string port);

    void Start();
    void Stop();

    const std::string& GetHost() const;
    const std::string& GetPort() const;

    Network::Websocket& Websocket();
    Network::HttpSession& Http();
};

} // namespace Server
} // namespace Model