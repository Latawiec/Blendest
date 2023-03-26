#include "./public/System/Process.hpp"

#include <boost/process.hpp>
#include <boost/asio.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <chrono>

#include <iostream>

using namespace std::chrono_literals;

namespace System {

namespace /* anonymous */ {

void read_loop(std::stringstream& output, boost::process::async_pipe& input, std::mutex& ioLock)
{
    static boost::asio::streambuf buffer;
    static std::string            line; // re-used because we can
    boost::asio::async_read_until(
        input, buffer, '\n',
        [&](boost::system::error_code ec, size_t /*bytes*/) {
            if (ec) {
                std::cerr << "Loop exit (" << ec.message() << ")" << std::endl;
                return;
            }
            if (getline(std::istream(&buffer), line)) {
                std::lock_guard<std::mutex> locked{ ioLock };
                output << line << std::endl;
            }

            read_loop(output, input, ioLock);
        }
    );
}

}

Process::Process(const std::string& exePath, const std::vector<std::string>& args, const std::string& cwd)
: _exePath(exePath)
, _args(args)
, _workingDirectory(cwd)
{}

Process::~Process()
{
    if (_process != nullptr && _process->running()) {
        _process->terminate();
        _ios->stop();
    }
}

std::future<int>& Process::Start()
{
    std::stringstream commandLineBuild;
    commandLineBuild << _exePath;
    for (const auto& arg : _args) {
        commandLineBuild << " " << arg;
    }

    _ios = std::make_shared<boost::asio::io_service>();
    boost::process::async_pipe bpStdOut(*_ios);
    boost::process::async_pipe bpStdErr(*_ios);
    try {
        _process = std::make_shared<boost::process::child>(commandLineBuild.str(), boost::process::std_out > bpStdOut, boost::process::std_err > bpStdErr);
    } catch (...) {
        auto promise = std::promise<int>();
        this->_return = promise.get_future();
        promise.set_value(1);
        return this->_return;
    }
    
    this->_return = std::async([&](
        std::shared_ptr<boost::process::child> process,
        std::shared_ptr<boost::asio::io_service> ios,
        boost::process::async_pipe stdOut,
        boost::process::async_pipe stdErr) -> int {

        try {
            read_loop(_stdErr, stdErr, _stdErrLock);
            read_loop(_stdOut, stdOut, _stdOutLock);

            while(process->running()) {
                ios->run_for(1s);
            }
            ios->run();

            return process->exit_code();
        } catch (std::exception e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }, _process, _ios, std::move(bpStdOut), std::move(bpStdErr)); 

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
    _stdOut.str(std::string());
    _stdOut.clear();
    return read;
}

std::string Process::ReadStdErr()
{
    std::lock_guard<std::mutex> lock{ _stdErrLock };
    const std::string read = _stdErr.str();
    _stdErr.str(std::string());
    _stdErr.clear();
    return read;
}


}