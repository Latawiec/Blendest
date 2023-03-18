#include "../public/Process.hpp"

#undef UNICODE
#include <windows.h>
#include <Accctrl.h>
#include <Aclapi.h>

#include <string>
#include <vector>

namespace /* anonymous */ {

    void TryCloseHandle(HANDLE& handle) 
    {
        if (handle != NULL) CloseHandle(handle);
        handle = NULL;
    }

    HANDLE createProcess(
        const std::string& commandLine,
        HANDLE& stdOut_Rd, HANDLE& stdOut_Wr,
        HANDLE& stdErr_Rd, HANDLE& stdErr_Wr)
    {
        // Create stdOut and stdErr pipes.
        {
            SECURITY_ATTRIBUTES saAttr;

            saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
            saAttr.bInheritHandle = TRUE;
            saAttr.lpSecurityDescriptor = NULL;

            // Create StdOut
            if (!CreatePipe(&stdOut_Rd, &stdOut_Wr, &saAttr, 0) ||
                !SetHandleInformation(stdOut_Rd, HANDLE_FLAG_INHERIT, 0))
            {
                TryCloseHandle(stdOut_Rd);
                TryCloseHandle(stdOut_Wr);
                return NULL;
            }

            // Create StdErr
            if (!CreatePipe(&stdErr_Rd, &stdErr_Wr, &saAttr, 0) ||
                !SetHandleInformation(stdErr_Rd, HANDLE_FLAG_INHERIT, 0))
            {
                TryCloseHandle(stdErr_Rd);
                TryCloseHandle(stdErr_Wr);
                return NULL;
            }

        }

        // Create process
        {
            PROCESS_INFORMATION piProcInfo;
            STARTUPINFO siStartInfo;
            BOOL bSuccess = FALSE;

            ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
            ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );

            siStartInfo.cb = sizeof(STARTUPINFO);
            siStartInfo.hStdError = stdErr_Wr;
            siStartInfo.hStdOutput = stdOut_Wr;
            siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

            bSuccess = CreateProcess(NULL,
                (CHAR*)commandLine.c_str(),  // command line
                NULL,                 // process security attributes 
                NULL,                 // primary thread security attributes 
                TRUE,                 // handles are inherited 
                0,                    // creation flags 
                NULL,                 // use parent's environment 
                NULL,                 // use parent's current directory 
                &siStartInfo,         // STARTUPINFO pointer 
                &piProcInfo);         // receives PROCESS_INFORMATION 

            if ( !bSuccess ) {
                TryCloseHandle(stdOut_Rd);
                TryCloseHandle(stdOut_Wr);
                TryCloseHandle(stdErr_Rd);
                TryCloseHandle(stdErr_Wr);
                return NULL;
            }
            
            // Close unused handles immediately.
            TryCloseHandle(piProcInfo.hThread);
            TryCloseHandle(stdOut_Wr);
            TryCloseHandle(stdErr_Wr);

            return piProcInfo.hProcess;
        }
        
    }

    void ReadFromPipe(HANDLE pipeHandle, std::stringstream& outputStream) 
    {
        constexpr int BUFSIZE = 512;
        DWORD dwRead = 0;
        CHAR chBuf[BUFSIZE]; 

        while( ReadFile( pipeHandle, chBuf, BUFSIZE, &dwRead, NULL) && (dwRead > 0) ) {
            outputStream.write(chBuf, dwRead);
        }
    }
}

namespace System {

Process::Process(const std::string& exePath, const std::vector<std::string>& args, const std::string& cwd)
: _exePath(exePath)
, _args(args)
, _workingDirectory(cwd)
{}

Process::~Process()
{
    TryCloseHandle(_handles._StdERR_Rd);
    TryCloseHandle(_handles._StdOUT_Rd);

    if (_handles._processHandle != NULL) {
        DWORD exitCode;
        GetExitCodeProcess(_handles._processHandle, &exitCode);
        if (exitCode == STILL_ACTIVE) {
            TerminateProcess(_handles._processHandle, 1);
        }
        CloseHandle(_handles._processHandle);
    }
}

std::future<int>& Process::Start()
{
    std::stringstream commandLineBuild;
    commandLineBuild << _exePath;
    for (const auto& arg : _args) {
        commandLineBuild << " " << arg;
    }

    HANDLE hStd_OUT_Rd = NULL;
    HANDLE hStd_OUT_Wr = NULL;
    HANDLE hStd_ERR_Rd = NULL;
    HANDLE hStd_ERR_Wr = NULL;
    _handles._processHandle = createProcess(commandLineBuild.str(), hStd_OUT_Rd, hStd_OUT_Wr, hStd_ERR_Rd, hStd_ERR_Wr);


    if (_handles._processHandle != NULL) {
        
        _handles._StdOUT_Rd = hStd_OUT_Rd;
        _handles._StdERR_Rd = hStd_ERR_Rd;
        
        this->_return = std::async([&](HANDLE processHandle, HANDLE stdOutRead, HANDLE stdErrRead) -> int {
            constexpr int timeout = 1000;
            while( true ) {
                {
                    std::lock_guard<std::mutex> lock{ _stdOutLock };
                    ReadFromPipe(stdOutRead, _stdOut);
                }
                {
                    std::lock_guard<std::mutex> lock{ _stdErrLock };
                    ReadFromPipe(stdErrRead, _stdErr);
                }
                auto res = WaitForSingleObject( processHandle, timeout );
                if (res != WAIT_TIMEOUT)
                    break;
            }
            
            DWORD exitCode = 1;                                  
            if (GetExitCodeProcess(processHandle, &exitCode)) {
                return exitCode;
            }
            return 1;
        }, _handles._processHandle, _handles._StdOUT_Rd, _handles._StdERR_Rd);
    } else {
        std::promise<int> promise;
        promise.set_value(1);
        this->_return = promise.get_future();
    }

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