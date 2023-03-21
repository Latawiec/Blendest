#pragma once

#include <string>
#include <vector>

namespace Model {

class ITask {

public:
    virtual ~ITask() = default;

    virtual const std::string& Command() const = 0;
    virtual const std::string& OutputPath() const = 0;
    virtual const std::vector<std::string>& AssetPaths() const = 0;

    virtual void OnProcessOutput(const std::string& stdOut, const std::string& stdErr) {};
};


}