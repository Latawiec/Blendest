#include "public/ViewModel/App.hpp"

#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive

namespace ViewModel {

App::App()
: _model{}
, _view{}
, _connectionViewModel(_model.Connection, _view.components.connection)
, _settingsViewModel(_view.components.settings)
{}

void App::Run() 
{
    auto screen = ftxui::ScreenInteractive::FitComponent();
    screen.Loop(_view.GetComponent());
}

}