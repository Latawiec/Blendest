#pragma once

#include <View/Component/Connection.hpp>
#include <View/Component/Settings.hpp>

#include <View/Component/TestButton.hpp>

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

        Component::TestButton testButton{"Test"};
    } components;

    Main();
    ftxui::Component GetComponent();
};

} // namespace View
} // namespace View