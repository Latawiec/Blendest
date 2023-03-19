#include "./public/App.hpp"

#include <views/Main.hpp>

#include <Connection.hpp>
#include <Settings.hpp>


void App::Run()
{
    UI::View::Main mainView;
    Model::Connection connectionModel(mainView.components.connection);
    Model::Settings settingsModel(mainView.components.settings);

    mainView.Loop();
}