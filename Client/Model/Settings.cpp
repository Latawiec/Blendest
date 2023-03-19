#include "./public/Settings.hpp"

#include <optional>
#include <chrono>
#include <regex>
#include <thread>

namespace Model {

namespace /* anonymous */ {
const std::string EmptyPathError = "Empty path";
const std::string WrongPathError = "Wrong path";
const std::string NotRespondingError = "Not responding";
const std::string VersionUnknownError = "Version unknown";
const std::regex BlenderVersionRegex("(Blender (?:.*))(?:\r\n)|(?:\n)");
}

Settings::Settings(UI::Component::Settings& view)
: _view(view)
{
    _view.setOnBlenderDirChange([&](const std::string& newPath) {

        if (opProcess.has_value()) opProcess = std::nullopt;

        if (newPath.empty()) {
            _view.setBlenderDirStatus(EmptyPathError);
            return;
        }

        if (!std::filesystem::exists(newPath)) {
            _view.setBlenderDirStatus(WrongPathError);
            return;   
        }

        std::filesystem::path blenderPath(newPath);
        if (std::filesystem::is_directory(blenderPath)) {
            blenderPath = blenderPath / "blender.exe";
            if (!std::filesystem::exists(blenderPath)) {
                _view.setBlenderDirStatus(WrongPathError);
                return;   
            }
        }

        std::filesystem::file_status status = std::filesystem::status(blenderPath);
        if ((status.permissions() & std::filesystem::perms::owner_exec) == std::filesystem::perms::none) {
            _view.setBlenderDirStatus(WrongPathError);
            return;
        }

        // It is blender, it is executable. Finally.
        opProcess.emplace(blenderPath.string(), std::vector<std::string>{ "--version" });
        std::future<int>& ret = opProcess.value().Start();

        using namespace std::chrono_literals;
        if ((ret.wait_for(5s) == std::future_status::ready) && ret.get() == 0) {
            std::string output = opProcess.value().ReadStdOut();
            std::smatch m;

            if (std::regex_search(output, m, BlenderVersionRegex) ){
                _view.setBlenderDirStatus(m[0].str());
            } else {
                _view.setBlenderDirStatus(VersionUnknownError);
            }
        } else {
            _view.setBlenderDirStatus(NotRespondingError);
            return;
        }
    });

    _view.setMaxThreadsCount(std::thread::hardware_concurrency());
    _view.setUtilizedThreadsCount(std::thread::hardware_concurrency());
}

}