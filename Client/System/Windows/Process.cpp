#include "../public/Process.hpp"

#include <boost/process/child.hpp>

#include <string>
#include <vector>

namespace System {

Process::Process(const std::string& exePath, const std::vector<std::string>& args, const std::string& cwd)
: _exePath(exePath)
, _args(args)
, _workingDirectory(cwd)
{}

Process::~Process()
{
    _process->terminate();
}

std::future<int>& Process::Start()
{
    std::stringstream commandLineBuild;
    commandLineBuild << _exePath;
    for (const auto& arg : _args) {
        commandLineBuild << " " << arg;
    }
    this->_return = std::async([&] {
        boost::process::ipstream bpStdOut;
        boost::process::ipstream bpStdErr;

        _process = std::make_shared<boost::process::child>(commandLineBuild.str(), boost::process::std_out > bpStdOut, boost::process::std_err > bpStdErr);
    
        while(!_process.wait_for(1s)) {
            std::string line;
            {
                std::lock_guard<std::mutex> lock{ _stdOutLock };
                while(std::getline(bStdOut, line)) {
                    _stdOut << line << std::endl;
                }
            }
            {
                std::lock_guard<std::mutex> lock{ _stdErrLock };
                while(std::getline(bStdErr, line)) {
                    _stdErr << line << std::endl;
                }
            }
        }
    });
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