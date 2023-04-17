#pragma once

#include <string>

namespace Model {
namespace Tasks {

enum class Status {
    None,
    Initializing,
    Running,
    Done,
    Canceled,
    Failed
};

class ITask {
public:
    virtual ~ITask() = default;

    virtual const std::string& GetTaskId() = 0;
    virtual Status GetStatus() = 0;
    virtual float GetProgress() = 0;
    virtual bool Run() = 0;
    virtual bool Cancel() = 0;
    virtual const std::string& GetErrorMessage() = 0;
};

}
}