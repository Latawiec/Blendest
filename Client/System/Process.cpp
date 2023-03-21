#include "./public/System/Process.hpp"

#include <boost/process.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <chrono>

#include <iostream>

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
    boost::process::child(commandLineBuild.str(), boost::process::std_out > bpStdOut, boost::process::std_err > bpStdErr);
    if (!_process->valid()) {
        auto promise = std::promise<int>();
        this->_return = promise.get_future();
        promise.set_value(1);
        return this->_return;
    }
    
    this->_return = std::async([&](boost::process::ipstream stdOut, boost::process::ipstream stdErr) -> int {
        // TODO: Fix this. I need to somehow move forward on the stream to not send same lanes again and again.
        const int MaxStdOutLines = 5;
        const int MaxStdErrLines = 50;
        const int ReadBatchSize = 128;
        std::string line(ReadBatchSize, '\0');
        do {
            {
                int linesLeft = MaxStdOutLines;
                std::lock_guard<std::mutex> lock{ _stdOutLock };
                while(stdOut.readsome(line.data(), ReadBatchSize) != 0) {
                    _stdOut << line << std::endl;
                    line.erase();
                }
            }
            {
                int linesLeft = MaxStdErrLines;
                std::lock_guard<std::mutex> lock{ _stdErrLock };
                stdErr.sync();
                while(linesLeft-- && (stdErr.readsome(line.data(), ReadBatchSize))) {
                    _stdErr << line << std::endl;
                    line.erase();
                }
            }
        } while(!_process->wait_for(1s));
        // Flush the IO one last time. Without any limits now. Just get everything out of there.
        {
            std::lock_guard<std::mutex> lock{ _stdOutLock };
            stdOut.sync();
            while(stdOut.getline(line.data(), ReadBatchSize)) {
                _stdOut << line << std::endl;
                line.erase();
            }
        }
        {
            std::lock_guard<std::mutex> lock{ _stdErrLock };
            stdErr.sync();
            while(stdErr.readsome(line.data(), ReadBatchSize)) {
                _stdErr << line << std::endl;
                line.erase();
            }
        }

        if (!_process->running()) {
            
            return _process->exit_code();
        }
        return 1;
    }, std::move(bpStdOut), std::move(bpStdErr)); 

    return this->_return;
}

void Process::Wait()
{
    this->_return.wait();
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