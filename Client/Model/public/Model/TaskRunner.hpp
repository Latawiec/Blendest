#pragma once

#include "ITask.hpp"
#include <System/Process.hpp>

#include <future>
#include <optional>

namespace Model {

class TaskRunner {

    ITask& _task;
    std::optional<System::Process> _taskProcess;
    bool _listening = false;
    std::promise<void> _processIO;
    std::future<void> _IOfinished;

public:
    TaskRunner(ITask& task);
    ~TaskRunner();

    void Start();
    void Wait();
    void Listen();
};

}