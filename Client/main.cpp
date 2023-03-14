#include <stdio.h>                  // for getchar
#include <ftxui/dom/elements.hpp>   // for filler, text, hbox, vbox
#include <ftxui/screen/screen.hpp>  // for Full, Screen
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include <memory>                   // for allocator
 
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for ftxui

#include "Settings.hpp"
#include "Connection.hpp"

#include "WebsocketConnection.hpp"
#include "IListener.hpp"
 #include <thread>


int main(int argc, const char* argv[]) {
  using namespace ftxui;

    struct PrinterListener : public Connection::IListener {

        virtual ~PrinterListener() = default;

        void OnConnect(const Connection::Error& error) override {
            //std::cout << "Connect: " << error.message << std::endl;
        }
        void OnRead(const Connection::Error& error, const Connection::Buffer& data) override {
            //std::cout << "Read: " << error.message << std::endl;
        }
        void OnClose(const Connection::Error& reason) override {
            //std::cout << "Close: " << reason.message << std::endl;
        }
    };

    PrinterListener listener{};
    Connection::WebsocketConnection wsConnection("localhost", 8080);
    wsConnection.AddListener(&listener);
    wsConnection.Start();

  UI::Settings settings;
  UI::Connection connection;

  auto settingsComponent = settings.getComponent();
  auto connectionComponent = connection.getComponent();

  auto component = Container::Horizontal({
    settingsComponent,
    connectionComponent
  });

  auto document =  Renderer(component, [&] {
      return hbox({
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