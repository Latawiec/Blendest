#pragma once

#include <sstream>
#include <future>
#include <thread>

// If windows..
#include <windows.h>

namespace System {

class Process {
    const std::string _exePath;
    const std::vector<std::string> _args;
    const std::string _workingDirectory;

    std::mutex _stdOutLock;
    std::mutex _stdErrLock;
    std::stringstream _stdOut;
    std::stringstream _stdErr;
    std::future<int> _return;
// If windows...
    HANDLE _processHandle;

public:
    Process(const std::string& exe, const std::vector<std::string>& args, const std::string& cwd = "");
    ~Process();

    std::future<int>& Start();
    std::string ReadStdOut();
    std::string ReadStdErr();
};

}