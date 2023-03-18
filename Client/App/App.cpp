#include "./public/App.hpp"

#include <components/Settings.hpp>
#include <components/Connection.hpp>
#include <views/Main.hpp>

#include <BackgroundWebsocket.hpp>
#include <IListener.hpp>
#include <Process.hpp>

#include <thread>
#include <optional>
#include <chrono>

using namespace std::chrono_literals;

void App::Run()
{
    using namespace ftxui;

    struct ConnectionListener : public Connection::IListener
    {
        UI::Component::Connection& connectionComponent;
        ConnectionListener(UI::Component::Connection& component)
        : connectionComponent(component) {}

        virtual ~ConnectionListener() = default;

        void OnError(const Connection::Error& error) override
        {
            connectionComponent.SetConnectionStatus(UI::Component::Connection::Status::RECONNECTING);
        }
        void OnMessage(const Connection::Buffer& data) override
        {
            connectionComponent.SetConnectionStatus(UI::Component::Connection::Status::CONNECTED);
        }
        void OnDestroy() override
        {
        }
    };

    std::optional<Connection::BackgroundWebsocket> wsConnectionOpt = std::nullopt;

    UI::View::Main mainView;
    ConnectionListener listener{mainView.components.connection};

    mainView.components.connection.OnConnect = [&](const std::string& host, const std::string& port) {
        wsConnectionOpt.emplace(host, port);
        wsConnectionOpt.value().RegisterListener(&listener);
        wsConnectionOpt.value().Listen();
    };

    mainView.components.connection.OnDisconnect = [&](const std::string& host, const std::string& port) {
        wsConnectionOpt.reset();
    };

    {
        System::Process p{ "C:\\Program Files\\Blender Foundation\\Blender 3.3\\blender.exe", { "--version"} };
        p.Start().wait();
        const std::string stdOut = p.ReadStdOut();
        const std::string stdErr = p.ReadStdErr();
    }


    mainView.Loop();
}