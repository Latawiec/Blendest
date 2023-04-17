#include "../public/Model/Tasks/Blender/RenderTask.hpp"


#include <chrono>
#include <regex>
#include <sstream>
#include <System/Process.hpp>

using namespace std::chrono_literals;

namespace Model {
namespace Tasks {
namespace Blender {

RenderTask::RenderTask(
    Model::Blender::Handle& blenderHandle,
    Model::Files::FileManager& fileManager,
    Model::Server::Connection& connection,
    const std::string& taskId,
    const std::string& downloadPath,
    const std::string& uploadPath)
: _blenderHandle(blenderHandle)
, _fileManager(fileManager)
, _connection(connection)
, _taskId(taskId)
, _blendFileServerPath(downloadPath)
, _outputServerPath(uploadPath)
, _taskProgress(Progress::Uninitialized)
{}

RenderTask::~RenderTask()
{

}

Status RenderTask::GetStatus() 
{
    switch(_taskProgress) {
        case Uninitialized:
            return Status::None;
        case FetchingFile:
        case ReadingFileInfo:
        case Initialized:
            return Status::Initializing;
        case Rendering:
        case UploadingResults:
            return Status::Running;
        case Done:
            return Status::Done;
        case Canceled:
            return Status::Canceled;
        case Failed:
            return Status::Failed;
    }
}

float RenderTask::GetProgress()
{
    return float(_framesDone) / _blendFileInfo.framesCount;
}

bool RenderTask::Run()
{
    if (_isCanceled) {
        return false;
    }

    if (!(
        createWorkDir() && 
        getBlendFile() &&
        readBlendFileInfo()
        ))
    {
        _taskProgress = Progress::Failed;
        return false;
    }
    _taskProgress = Progress::Initialized;

    const std::string blenderExePath = _blenderHandle.GetPath().generic_string();
    const std::string outputPath = (_taskWorkDir / _outputDir / "output_").generic_string();

    System::Process blenderProcess { blenderExePath, { "-b", _blendFileLocalPath, "-o", outputPath, "-a"} };
    std::future<int>& result = blenderProcess.Start();

    while (!_isCanceled && result.wait_for(1s) == std::future_status::timeout) {
        const std::string output = blenderProcess.ReadStdOut();
        const std::regex frameFinishedRegex("^Fra:(\\d+).*Finished$");
        std::smatch m;

        if (std::regex_search(output, m, frameFinishedRegex)) {
            _framesDone++;
        }
    }

    if (_isCanceled) {
        _taskProgress = Progress::Canceled;
        return true;
    }

    if (result.get() != 0) {
        _taskProgress = Progress::Failed;
        return false;
    }

    if (!uploadResults()) {
        _taskProgress = Progress::Failed;
        return false;
    }

    _taskProgress = Progress::Done;
    return true;
}

bool RenderTask::Cancel()
{
    _isCanceled = true;
    return true;
}

const std::string& RenderTask::GetErrorMessage()
{
    return _errorMessage;
}


bool RenderTask::createWorkDir()
{
    _taskWorkDir = _fileManager.GetTmpStorageDir() / _taskId;
    std::filesystem::create_directory(_taskWorkDir);
    std::filesystem::create_directory(_taskWorkDir / _outputDir);
    std::filesystem::create_directory(_taskWorkDir / _cacheDir);

    return true;
}

bool RenderTask::getBlendFile()
{
    _taskProgress = Progress::FetchingFile;

    const auto filename = std::filesystem::path(_blendFileServerPath).filename();
    _blendFileLocalPath = (_taskWorkDir / _cacheDir / filename).generic_string();

    auto fileFuture = _connection.Http().GetFile(_blendFileServerPath, _blendFileLocalPath);

    std::future_status status;
    switch (status = fileFuture.wait_for(_fetchFileTimeout), status) {
        default:
            return false;

        case std::future_status::timeout:
            _errorMessage = "Fetching file timed out.";
            _taskProgress = Progress::Failed;
            return false;
            break;

        case std::future_status::ready: 
            auto error = fileFuture.get();
            if (error) {
                _errorMessage = std::move(error.message);
                _taskProgress = Progress::Failed;
                return false;
            }
            break;
    };

    return true;
}

namespace /* anonymous */ {
const std::regex BlendFileInfoRegex(
R"(
^Scene: (.*)$
^First frame: (.*)$
^Last frame: (.*)$
^Total frames: (.*)$
)");

}

bool RenderTask::readBlendFileInfo()
{
    _taskProgress = Progress::ReadingFileInfo;

    const std::string blendFileInfoScriptPath = (_fileManager.GetAssetStorageDir() / "BlenderFileInfo.py").generic_string();
    const std::string blenderExePath = _blenderHandle.GetPath().generic_string();

    System::Process blenderProcess{ blenderExePath, { "-b", "-P", blendFileInfoScriptPath } };
    std::future<int>& blenderProcessFuture = blenderProcess.Start();

    if ((blenderProcessFuture.wait_for(_readFileDataTimeout) == std::future_status::ready) && blenderProcessFuture.get() == 0 ) {
        std::string output = blenderProcess.ReadStdOut();
        std::smatch m;

        if (std::regex_search(output, m, BlendFileInfoRegex)) {
            _blendFileInfo = {
                .sceneName = m[1].str(),
                .startFrame = std::stoi(m[2].str()),
                .endFrame = std::stoi(m[3].str()),
                .framesCount = std::stoi(m[4].str())
            };

            return true;
        }

        return false;
    }

    return false;
}

bool RenderTask::uploadResults()
{
    _taskProgress = Progress::UploadingResults;
    // TODO
    return true;
}


}
}
}