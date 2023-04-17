#include "public/View/View/Main.hpp"

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include <string>

using namespace ftxui;

namespace View {
namespace View {

Main::Main()
 {
    _view = ftxui::Container::Horizontal({
        components.settings.GetComponent(),
        components.connection.GetComponent(),
        components.testButton.GetComponent()
    });
}

ftxui::Component Main::GetComponent()
{
    return _view;
}

} // namespace View
} // namespace View
