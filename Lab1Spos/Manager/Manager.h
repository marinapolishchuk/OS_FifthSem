#pragma once

#include <Utility/Process/Process.hpp>
#include <Utility/Pipe/NamedPipe.hpp>

class Manager
{
public:
    Manager(const std::string& cmd);

    int Run();
    double GetComputationResult();

private:
    std::shared_ptr<Process> fProcess;
    std::shared_ptr<Process> gProcess;

    std::shared_ptr<NamedPipe> fPipe;
    std::shared_ptr<NamedPipe> gPipe;

    double result = 0;
};

