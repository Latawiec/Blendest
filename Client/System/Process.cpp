#include "./public/Process.hpp"

#include <boost/process.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

namespace /* anonymous */ {

    // void TryCloseHandle(HANDLE& handle) 
    // {
    //     if (handle != NULL) CloseHandle(handle);
    //     handle = NULL;
    // }

    // HANDLE createProcess(
    //     const std::string& commandLine,
    //     HANDLE& stdOut_Rd, HANDLE& stdOut_Wr,
    //     HANDLE& stdErr_Rd, HANDLE& stdErr_Wr)
    // {
    //     // Create stdOut and stdErr pipes.
    //     {
    //         SECURITY_ATTRIBUTES saAttr;

    //         saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    //         saAttr.bInheritHandle = TRUE;
    //         saAttr.lpSecurityDescriptor = NULL;

    //         // Create StdOut
    //         if (!CreatePipe(&stdOut_Rd, &stdOut_Wr, &saAttr, 0) ||
    //             !SetHandleInformation(stdOut_Rd, HANDLE_FLAG_INHERIT, 0))
    //         {
    //             TryCloseHandle(stdOut_Rd);
    //             TryCloseHandle(stdOut_Wr);
    //             return NULL;
    //         }

    //         // Create StdErr
    //         if (!CreatePipe(&stdErr_Rd, &stdErr_Wr, &saAttr, 0) ||
    //             !SetHandleInformation(stdErr_Rd, HANDLE_FLAG_INHERIT, 0))
    //         {
    //             TryCloseHandle(stdErr_Rd);
    //             TryCloseHandle(stdErr_Wr);
    //             return NULL;
    //         }

    //     }

    //     // Create process
    //     {
    //         PROCESS_INFORMATION piProcInfo;
    //         STARTUPINFO siStartInfo;
    //         BOOL bSuccess = FALSE;

    //         ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
    //         ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );

    //         siStartInfo.cb = sizeof(STARTUPINFO);
    //         siStartInfo.hStdError = stdErr_Wr;
    //         siStartInfo.hStdOutput = stdOut_Wr;
    //         siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    //         bSuccess = CreateProcess(NULL,
    //             (CHAR*)commandLine.c_str(),  // command line
    //             NULL,                 // process security attributes 
    //             NULL,                 // primary thread security attributes 
    //             TRUE,                 // handles are inherited 
    //             0,                    // creation flags 
    //             NULL,                 // use parent's environment 
    //             NULL,                 // use parent's current directory 
    //             &siStartInfo,         // STARTUPINFO pointer 
    //             &piProcInfo);         // receives PROCESS_INFORMATION 

    //         if ( !bSuccess ) {
    //             TryCloseHandle(stdOut_Rd);
    //             TryCloseHandle(stdOut_Wr);
    //             TryCloseHandle(stdErr_Rd);
    //             TryCloseHandle(stdErr_Wr);
    //             return NULL;
    //         }
            
    //         // Close unused handles immediately.
    //         TryCloseHandle(piProcInfo.hThread);
    //         TryCloseHandle(stdOut_Wr);
    //         TryCloseHandle(stdErr_Wr);

    //         return piProcInfo.hProcess;
    //     }
        
    // }

    // void ReadFromPipe(HANDLE pipeHandle, std::stringstream& outputStream) 
    // {
    //     constexpr int BUFSIZE = 512;
    //     DWORD dwRead = 0;
    //     CHAR chBuf[BUFSIZE]; 

    //     while( ReadFile( pipeHandle, chBuf, BUFSIZE, &dwRead, NULL) && (dwRead > 0) ) {
    //         outputStream.write(chBuf, dwRead);
    //     }
    // }
}

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