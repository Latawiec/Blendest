#pragma once

#include <Model/Main/Main.hpp>
#include <View/View/Main.hpp>

#include <ViewModel/Connection.hpp>
#include <ViewModel/Settings.hpp>

#include "Connection.hpp"

namespace ViewModel {

class App {
    Model::Main::Main _model;
    View::View::Main _view;

    Connection _connectionViewModel;
    Settings   _settingsViewModel;
public:
    App();

    void Run();
};

} // namespace ViewModel