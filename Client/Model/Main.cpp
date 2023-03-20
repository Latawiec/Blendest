#include "public/Model/Main.hpp"

namespace Model {

Main::Main(UI::View::Main& mainView)
: _view(mainView)
, connectionModel(mainView.components.connection)
, settingsModel(mainView.components.settings)
{}

}