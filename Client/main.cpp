#include <stdio.h>                  // for getchar
#include <ftxui/dom/elements.hpp>   // for filler, text, hbox, vbox
#include <ftxui/screen/screen.hpp>  // for Full, Screen
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include <memory>                   // for allocator
 
#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for ftxui

#include "Settings.hpp"
 #include <thread>


int main(int argc, const char* argv[]) {
  using namespace ftxui;

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