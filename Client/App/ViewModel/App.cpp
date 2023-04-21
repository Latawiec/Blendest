#include "public/ViewModel/App.hpp"

#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive

#include <ViewModel/Serializer/Serializer.hpp>

#include <Model/Tasks/Blender/RenderTask.hpp>

namespace ViewModel {

App::App()
: _model{}
, _view{}
, _connectionViewModel(_model.Connection, _view.components.connection)
, _settingsViewModel(_model.BlenderSettings, _view.components.settings)
{}

void App::Run() 
{
    // TEST
    {
        _view.components.testButton.OnClick([&] {
            _model.Connection.Http().SendFile("/send", "C:\\Users\\lmarz\\Documents\\TexTools\\Saved\\FullModel\\Frustum\\mt_c0101e0684_glv_a_s.png");
            // Model::Tasks::Blender::RenderTask task {
            //     _model.BlenderSettings.GetHandle().value(),
            //     _model.FileManager,
            //     _model.Connection,
            //     "testTask",
            //     "test/test.blend",
            //     ""
            // };
            // task.Run();
        });
    }


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