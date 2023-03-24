#include "public/Model/Connection.hpp"

#include "public/Model/BlenderTask.hpp"
#include "public/Model/TaskRunner.hpp"

#include <chrono>
#include <string>
#include <iostream>
using namespace std::chrono_literals;

namespace Model {

Connection::WebsocketListener::WebsocketListener(UI::Component::Connection& view)
: connectionComponent(view)
{}

Connection::WebsocketListener::~WebsocketListener()
{}


void Connection::WebsocketListener::OnError(const Network::Error& error)
{
    connectionComponent.SetConnectionStatus(UI::Component::Connection::Status::RECONNECTING);
}

void Connection::WebsocketListener::OnMessage(const Network::Buffer& data)
{
    connectionComponent.SetConnectionStatus(UI::Component::Connection::Status::CONNECTED);
    std::cout << reinterpret_cast<const char*>(data.data) << std::endl;
}


Connection::Connection(UI::Component::Connection& connectionView)
: _view(connectionView)
, _wsListener(_view)
{
    _view.OnConnectClick = [&](const std::string& host, const std::string& port) {
        wsConnectionOpt.emplace(host, port);
        wsConnectionOpt.value().RegisterListener(&_wsListener);
        wsConnectionOpt.value().Listen();
        _data.hostAddress = host;
        _data.hostPort = port;

        for (int i=0; i < 100; ++i) {
            wsConnectionOpt.value().Write(std::string("Value is: ") + std::to_string(i));
            std::this_thread::sleep_for(1s);
        }

// EXAMPLE
//         httpSession.emplace(host);
//         httpSession.value().Start();
//         auto fut = httpSession.value().GetFile("/file/someData.json", "./someData.json");
//         fut.wait();
//         auto val = fut.get();
// // EXAMPLE

// // ANOTHER EXAMPLE, I LIKE THIS PLACE IN CODE
//         BlenderTask task(
//             "C:/Program Files/Blender Foundation/Blender 3.3/blender.exe",
//             "D:/tmp/tmp/Carbuncle.blend",
//             "D:/tmp/tmp/",
//             "fraame_####"
//         );

//         TaskRunner runner(task);
//         runner.Start();
//         runner.Listen();
//         runner.Wait();
// ANOTHER EXAMPLE
    };

    _view.OnDisconnectClick = [&](const std::string& host, const std::string& port) {
        wsConnectionOpt.reset();
        httpSession.reset();
    };
}

Connection::~Connection() {}

const Connection::Data& Connection::getData() const
{
    return this->_data;
}

void Connection::setData(Connection::Data newData)
{
    _view.setHostAddress(newData.hostAddress);
    _view.setHostPort(newData.hostPort);
    this->_data = std::move(newData);
}

}