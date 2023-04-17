#pragma once

#include "../ITask.hpp"

#include <Model/Files/FileManager.hpp>
#include <Model/Server/Connection.hpp>
#include <Model/Blender/Handle.hpp>

#include <memory>
#include <chrono>

using namespace std::chrono_literals;

namespace Model {

namespace Blender {
class Handle;
}

namespace Tasks {
namespace Blender { 

class RenderTask : public ITask {

    struct BlendFileInfo {
        std::string sceneName;
        int startFrame;
        int endFrame;
        int framesCount;
    } _blendFileInfo;

    enum Progress {
        Uninitialized,
        FetchingFile,
        ReadingFileInfo,
        Initialized,
        Rendering,
        UploadingResults,
        Done,
        Canceled,
        Failed
    };
    
    Progress                   _taskProgress;
    Model::Blender::Handle&    _blenderHandle;
    Model::Files::FileManager& _fileManager;
    Model::Server::Connection& _connection;


    std::filesystem::path _taskWorkDir;
    const std::filesystem::path _outputDir = "Output";
    const std::filesystem::path _cacheDir  = "Cache";

    const std::string _taskId;
    const std::string _blendFileServerPath;
    std::string       _blendFileLocalPath;

    std::string _errorMessage = "";

    bool _isCanceled = false;

    const std::chrono::seconds _fetchFileTimeout = 5min;
    const std::chrono::seconds _readFileDataTimeout = 30s;

    bool createWorkDir();
    bool getBlendFile();
    bool readBlendFileInfo();

public:
    struct Parameters {
        int startFrame  = -1;
        int endFrame    = -1;
        int threadCount = -1;
    };

    RenderTask(
        Model::Blender::Handle& blenderHandle,
        Model::Files::FileManager& fileManager,
        Model::Server::Connection& connection,
        const std::string& taskId,
        const std::string& assetPath
    );

    virtual ~RenderTask();

    Status GetStatus() override;
    float GetProgress() override;
    bool Run() override;
    bool Cancel() override;
    const std::string& GetErrorMessage() override;
};

}
}
}