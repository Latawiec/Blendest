#include <stdio.h>                  // for getchar
#include <ftxui/dom/elements.hpp>   // for filler, text, hbox, vbox
#include <ftxui/screen/screen.hpp>  // for Full, Screen
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include <memory>                   // for allocator
 
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for ftxui

#include "Settings.hpp"

#include "ServerConnection.hpp"
 #include <thread>


int main(int argc, const char* argv[]) {
  using namespace ftxui;

    struct PrinterListener : public Connection::ServerConnection::IListener {
        PrinterListener(const Connection::ServerConnection* connection)
        : Connection::ServerConnection::IListener(connection) {}

        virtual ~PrinterListener() = default;

        void OnConnect(const boost::system::error_code& error) override {
            std::cout << "Connect: " << error.what() << std::endl;
        }
        void OnRead(const boost::system::error_code& error, const boost::beast::flat_buffer& data) override {
            std::cout << "Read: " << error.what() << std::endl;
        }
        void OnClose(const boost::beast::websocket::close_reason& reason) override {
            std::cout << "Close: " << reason.reason << std::endl;
        }
    };

    Connection::ServerConnection connection("localhost", 8080);
    connection.Start();

    PrinterListener listener{&connection};
  Settings settings;

  auto settingsComponent = settings.getComponent();

  auto document =  Renderer(settingsComponent, [&] {
      return vbox({
          hbox({
              text("north-west"),
              filler(),
              text("north-east"),
          }),
          filler(),
          hbox({
              filler(),
              text("center"),
              filler(),
          }),
          filler(),
          hbox({
              text("south-west"),
              filler(),
              text("south-east"),
          }),
          settingsComponent->Render()
      });
    });
//   auto screen = Screen::Create(Dimension::Full());
//   Render(screen, document);
  // screen.Print();
  // getchar();
  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(document);
  return 0;
}
 
// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.