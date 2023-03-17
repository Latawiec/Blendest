#include "./public/App.hpp"

#include <stdio.h>                                // for getchar
#include <ftxui/dom/elements.hpp>                 // for filler, text, hbox, vbox
#include <ftxui/screen/screen.hpp>                // for Full, Screen
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include <memory>                                 // for allocator

#include "ftxui/dom/node.hpp"     // for Render
#include "ftxui/screen/color.hpp" // for ftxui

#include "Settings.hpp"
#include "Connection.hpp"

#include "BackgroundWebsocket.hpp"
#include "IListener.hpp"
#include <thread>
#include <optional>

void App::Run()
{
    using namespace ftxui;

    struct ConnectionListener : public Connection::IListener
    {
        UI::Connection& connectionComponent;
        ConnectionListener(UI::Connection& component)
        : connectionComponent(component) {}

        virtual ~ConnectionListener() = default;

        void OnError(const Connection::Error& error) override
        {
            connectionComponent.SetConnectionStatus(UI::Connection::Status::RECONNECTING);
        }
        void OnMessage(const Connection::Buffer& data) override
        {
            connectionComponent.SetConnectionStatus(UI::Connection::Status::CONNECTED);
        }
        void OnDestroy() override
        {
        }
    };

    std::optional<Connection::BackgroundWebsocket> wsConnectionOpt = std::nullopt;

    UI::Settings settings;
    UI::Connection connection;
    ConnectionListener listener{connection};

    connection.OnConnect = [&](const std::string& host, const std::string& port) {
        wsConnectionOpt.emplace(host, port);
        wsConnectionOpt.value().RegisterListener(&listener);
        wsConnectionOpt.value().Listen();
    };

    connection.OnDisconnect = [&](const std::string& host, const std::string& port) {
        wsConnectionOpt.reset();
    };

    auto settingsComponent = settings.getComponent();
    auto connectionComponent = connection.getComponent();

    auto component = Container::Horizontal({settingsComponent,
                                            connectionComponent});

    auto document = Renderer(component, [&]
                             { return hbox({
                                   //   hbox({
                                   //       text("north-west"),
                                   //       filler(),
                                   //       text("north-east"),
                                   //   }),
                                   //   filler(),
                                   //   hbox({
                                   //       filler(),
                                   //       text("center"),
                                   //       filler(),
                                   //   }),
                                   //   filler(),
                                   //   hbox({
                                   //       text("south-west"),
                                   //       filler(),
                                   //       text("south-east"),
                                   //   }),
                                   component->Render(),
                               }); });
    //   auto screen = Screen::Create(Dimension::Full());
    //   Render(screen, document);
    // screen.Print();
    // getchar();
    auto screen = ScreenInteractive::FitComponent();
    screen.Loop(document);
}