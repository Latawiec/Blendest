#include "public/Model/TaskRunner.hpp"

#include <chrono>

using namespace std::chrono_literals;

namespace Model {

TaskRunner::TaskRunner(ITask& task)
: _task(task)
{}

TaskRunner::~TaskRunner()
{
    _processIO.set_value();
    _IOfinished.wait();
}

void TaskRunner::Start() {
    _taskProcess.emplace(_task.Command(), std::vector<std::string>{});
    _taskProcess->Start();
}

void TaskRunner::Wait() {
    if (_taskProcess) {
        _taskProcess->Wait();
    }
}

void TaskRunner::Listen() {
    if (_listening) {
        _processIO.set_value();
        _IOfinished.wait();
    }

    _processIO = std::promise<void>();
    _IOfinished = std::async([this] {
        const auto future = _processIO.get_future();
        while (future.wait_for(1s) != std::future_status::ready) {
            if (_taskProcess.has_value()) {
                const std::string stdOut = _taskProcess.value().ReadStdOut();
                const std::string stdErr = _taskProcess.value().ReadStdErr();

                _task.OnProcessOutput(stdOut, stdErr);
            }
        }
    });
    _listening = true;
}

}