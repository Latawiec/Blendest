#pragma once

#include <sstream>
#include <future>
#include <thread>

namespace boost {
namespace process {
    class child;
}
}

namespace boost {
namespace asio {
    class io_context;
}
}

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

    std::shared_ptr<boost::process::child>  _process;
    std::shared_ptr<boost::asio::io_context> _ios;

public:
    Process(const std::string& exe, const std::vector<std::string>& args, const std::string& cwd = "");
    ~Process();

    std::future<int>& Start();
    void Wait();
    std::string ReadStdOut();
    std::string ReadStdErr();
};

}