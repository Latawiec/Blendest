#pragma once

#include <View/Component/Connection.hpp>
#include <View/Component/Settings.hpp>

#include <memory>

namespace ftxui {
class ComponentBase;
using Component = std::shared_ptr<ftxui::ComponentBase>;
}

namespace View {
namespace View {

class Main {

    ftxui::Component _view;
public:
    struct {
        Component::Connection connection;
        Component::Settings   settings;
    } components;

    Main();
    ftxui::Component GetComponent();
};

} // namespace View
} // namespace View