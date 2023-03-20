#include "./public/UI/View/Main.hpp"

#include <ftxui/dom/elements.hpp>                 // for filler, text, hbox, vbox
#include <ftxui/screen/screen.hpp>                // for Full, Screen
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/component/component.hpp" // for ScreenInteractive

using namespace ftxui;

namespace UI {
namespace View {

Main::Main() {
    _document = ftxui::Container::Horizontal({
        components.settings.getComponent(),
        components.connection.getComponent()
    });
}

void Main::Loop() {
    auto screen = ScreenInteractive::FitComponent();
    
    screen.Loop(Renderer(_document, [&] {
        OnRender();
        return _document->Render();
    }));
}

}
}