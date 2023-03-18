#pragma once

#include <memory>

#include "../components/Settings.hpp"
#include "../components/Connection.hpp"

namespace ftxui {
class ComponentBase;
}


namespace UI {
namespace View {

class Main {
    std::shared_ptr<ftxui::ComponentBase> _document;

public:
    struct {
        Component::Connection connection;
        Component::Settings settings;
    } components;

    std::function<void()> OnRender = []{};

    Main();

    void Loop();
};

}
}