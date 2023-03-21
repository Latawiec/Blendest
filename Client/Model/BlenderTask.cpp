#include "public/Model/BlenderTask.hpp"

#include <filesystem>
#include <regex>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace Model {

BlenderTask::BlenderTask(
    const std::string& blenderExePath,
    const std::string& blendFilePath,
    const std::string& outputPath,
    const std::string& outputFormat)
: _blenderExePath(blenderExePath)
, _blendFilePath(blendFilePath)
, _outputPath(outputPath)
, _outputFormat(outputFormat)
{
    updateCommand();
}

const std::string& BlenderTask::Command() const
{
    return this->_command;
}

const std::string& BlenderTask::OutputPath() const
{
    return this->_outputPath;
}

const std::vector<std::string>& BlenderTask::AssetPaths() const
{
    return { this->_blendFilePath };
}

void BlenderTask::OnProcessOutput(const std::string& stdOut, const std::string& stdErr)
{
    const std::regex FrameDoneRegex("(?:Fra:((?:\\d)+) .*$)(?:\\n|\\r\\n)Saved: '(.*)'(?:\\n|\\r\\n) Time: (\\d\\d:\\d\\d.\\d\\d) ");
    
    auto framesDone_begin = std::sregex_iterator(stdOut.begin(), stdOut.end(), FrameDoneRegex);
    auto framesDone_end = std::sregex_iterator();

    for (auto i = framesDone_begin; i != framesDone_end; ++i) {
        const std::smatch& matches = *i;
        std::cout << "Frame done: " << matches[1].str() << " saved as: " << matches[2].str() << " time elapsed: " << matches[3].str() << std::endl;
    }
}

void BlenderTask::SetStartFrame(std::optional<int> frame)
{
    this->_startFrame = frame;
    updateCommand();
}

void BlenderTask::SetEndFrame(std::optional<int> frame)
{
    this->_endFrame = frame;
    updateCommand();
}

void BlenderTask::SetThreadsCount(std::optional<int> count)
{
    this->_threadsCount = count;
    updateCommand();
}

void BlenderTask::SetClampTimeline(bool clamp)
{
    this->_clampToTimeline = clamp;
    updateCommand();
}



namespace /* anonymous */ {
namespace BlenderArgs {
static const std::string NoUI = "-b";
static const std::string AddFileExtension = "-x";
static const std::string RenderOutput = "--render-output";
static const std::string RenderTimeline = "--render-anim";
static const std::string StartFrame = "--frame-start";
static const std::string EndFrame = "--frame-end";
static const std::string ThreadsCount = "--threads";
};
}

void BlenderTask::updateCommand()
{
    std::stringstream ssCommand;
    ssCommand << std::quoted(_blenderExePath) << " "
        << BlenderArgs::NoUI << " "
        << BlenderArgs::AddFileExtension << " 1 "
        << std::quoted(_blendFilePath) << " "
        << BlenderArgs::RenderOutput << " " << std::quoted((std::filesystem::path(_outputPath) / _outputFormat).string()) << " "
        << (_threadsCount ? (BlenderArgs::ThreadsCount + " " + std::to_string(_threadsCount.value())) : "") << " "
        << (_clampToTimeline ? BlenderArgs::RenderTimeline : "") + " "
        << (_startFrame ? (BlenderArgs::StartFrame + " " + std::to_string(_startFrame.value())) : "") << " "
        << (_endFrame ? (BlenderArgs::EndFrame + " " + std::to_string(_endFrame.value())) : "") << " "
        ;

    _command = ssCommand.str();
}

}