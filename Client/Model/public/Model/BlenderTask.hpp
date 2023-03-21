#pragma once

#include "ITask.hpp"
#include <functional>
#include <optional>

namespace Model {

class BlenderTask : public ITask {
    const std::string _blenderExePath;
    const std::string _blendFilePath;
    const std::string _outputPath;
    const std::string _outputFormat;

    std::string _command;

    std::optional<int> _startFrame;
    std::optional<int> _endFrame;
    std::optional<int> _threadsCount;
    bool _clampToTimeline = true;

public:
    BlenderTask(
        const std::string& blenderExePath,
        const std::string& blenFilePath,
        const std::string& outputPath,
        const std::string& outputFormat = "frame_#####");

    virtual ~BlenderTask() = default;

    const std::string& Command() const override;
    const std::string& OutputPath() const override;
    const std::vector<std::string>& AssetPaths() const override;
    void OnProcessOutput(const std::string& stdOut, const std::string& stdErr) override;

    void SetStartFrame(std::optional<int> frame);
    void SetEndFrame(std::optional<int> frame);
    void SetThreadsCount(std::optional<int> count);
    void SetClampTimeline(bool clamp);

    std::function<void(int)> OnFrameDone;

private:
    void updateCommand();
};

}