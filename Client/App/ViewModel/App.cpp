#include "public/ViewModel/App.hpp"

#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive

#include <ViewModel/Serializer/Serializer.hpp>

namespace ViewModel {

App::App()
: _model{}
, _view{}
, _connectionViewModel(_model.Connection, _view.components.connection)
, _settingsViewModel(_view.components.settings)
{}

void App::Run() 
{
    Serializer::Serializer serializer(_model.FileManager);

    serializer.Deserialize(_view);

    auto screen = ftxui::ScreenInteractive::FitComponent();
    screen.Loop(_view.GetComponent());

    struct OnExitTrySerialize {
        Serializer::Serializer& _serializer;
        View::View::Main& _view;
        OnExitTrySerialize(Serializer::Serializer& serializer, View::View::Main& view)
        : _serializer(serializer)
        , _view(view)
        {}

        ~OnExitTrySerialize() {
            _serializer.Serialize(_view);
        }
    } raii(serializer, _view);
}

}