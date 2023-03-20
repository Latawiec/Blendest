#pragma once

#include <UI/View/Main.hpp>

#include "Connection.hpp"
#include "Settings.hpp"

namespace Model {

class Main {

    UI::View::Main& _view;


public:
    Model::Connection connectionModel;
    Model::Settings settingsModel;

    Main(UI::View::Main& mainView);
};

}