#include "./public/Process.hpp"

#include <boost/process.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

namespace System {

Process::Process(const std::string& exePath, const std::vector<std::string>& args, const std::string& cwd)
: _exePath(exePath)
, _args(args)
, _workingDirectory(cwd)
{}

Process::~Process()
{
    if (_process != nullptr && _process->running()) {
        _process->terminate();
    }
}

std::future<int>& Process::Start()
{
    std::stringstream commandLineBuild;
    commandLineBuild << _exePath;
    for (const auto& arg : _args) {
        commandLineBuild << " " << arg;
    }
    boost::process::ipstream bpStdOut;
    boost::process::ipstream bpStdErr;

    _process = make_shared<boost::process::child>(commandLineBuild.str(), boost::process::std_out > bpStdOut, boost::process::std_err > bpStdErr);
    if (!_process->valid()) {
        auto promise = std::promise<int>();
        this->_return = promise.get_future();
        promise.set_value(1);
        return this->_return;
    }

    this->_return = std::async([&](boost::process::ipstream stdOut, boost::process::ipstream stdErr) -> int {
        do {
            std::string line;
            {
                std::lock_guard<std::mutex> lock{ _stdOutLock };
                while(std::getline(stdOut, line)) {
                    _stdOut << line << std::endl;
                }
            }
            {
                std::lock_guard<std::mutex> lock{ _stdErrLock };
                while(std::getline(stdErr, line)) {
                    _stdErr << line << std::endl;
                }
            }
        } while(!_process->wait_for(1s));

        if (!_process->running()) {
            return _process->exit_code();
        }
        return 1;
    }, std::move(bpStdOut), std::move(bpStdErr)); 

    return this->_return;
}

std::string Process::ReadStdOut()
{
    std::lock_guard<std::mutex> lock{ _stdOutLock };
    const std::string read = _stdOut.str();
    _stdOut.clear();
    return read;
}

std::string Process::ReadStdErr()
{
    std::lock_guard<std::mutex> lock{ _stdErrLock };
    const std::string read = _stdErr.str();
    _stdErr.clear();
    return read;
}


}