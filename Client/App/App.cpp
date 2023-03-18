#include "./public/App.hpp"

#include <components/Settings.hpp>
#include <components/Connection.hpp>
#include <views/Main.hpp>

#include <BackgroundWebsocket.hpp>
#include <IListener.hpp>
#include <Process.hpp>
#include <Connection.hpp>

#include <thread>
#include <optional>
#include <chrono>
#include <regex>

using namespace std::chrono_literals;

void App::Run()
{
    UI::View::Main mainView;
    Model::Connection connectionModel(mainView.components.connection);

    std::optional<System::Process> opProcess = std::nullopt;
    mainView.components.settings.setOnBlenderDirChange([&](const std::string& newPath) {
        if (opProcess.has_value()) opProcess = std::nullopt;

        if (newPath.empty()) {
            mainView.components.settings.setBlenderDirStatus("Empty path");
            return;
        }

        if (!std::filesystem::exists(newPath)) {
            mainView.components.settings.setBlenderDirStatus("Wrong path");
            return;   
        }

        std::filesystem::path blenderPath(newPath);
        if (std::filesystem::is_directory(blenderPath)) {
            blenderPath = blenderPath / "blender.exe";
            if (!std::filesystem::exists(blenderPath)) {
                mainView.components.settings.setBlenderDirStatus("Wrong path");
                return;   
            }
        }

        std::filesystem::file_status status = std::filesystem::status(blenderPath);
        if ((status.permissions() & std::filesystem::perms::owner_exec) == std::filesystem::perms::none) {
            mainView.components.settings.setBlenderDirStatus("Wrong path");
            return;
        }

        // It is blender, it is executable. Finally.
        opProcess.emplace(blenderPath.string(), std::vector<std::string>{ "--version" });
        std::future<int>& ret = opProcess.value().Start();

        using namespace std::chrono_literals;
        if ((ret.wait_for(5s) == std::future_status::ready) && ret.get() == 0) {
            std::string output = opProcess.value().ReadStdOut();

            std::regex blenderVersionRegex("(Blender (?:.*))(?:\r\n)|(?:\n)");
            std::smatch m;

            if (std::regex_search(output, m, blenderVersionRegex) ){
                mainView.components.settings.setBlenderDirStatus(m[0].str());
            } else {
                mainView.components.settings.setBlenderDirStatus("Version unknown");
            }
        } else {
            mainView.components.settings.setBlenderDirStatus("Not responding");
            return;
        }
    });

    {
        System::Process p{ "C:\\Program Files\\Blender Foundation\\Blender 3.3\\blender.exe", { "--version"} };
        p.Start().wait();
        const std::string stdOut = p.ReadStdOut();
        const std::string stdErr = p.ReadStdErr();
    }


    mainView.Loop();
}