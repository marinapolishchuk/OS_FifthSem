#pragma once

#include <memory>
#include <string>

class Process
{
private:
    struct Impl;
public:
    Process(std::unique_ptr<Impl>& impl);
    Process(Process&&) = default;
    ~Process();

    static std::shared_ptr<Process> Create(const std::string& name, std::string cmd);

    void Run();
    void Join();
    void Terminate();

    int GetExitCode();

private:
    std::unique_ptr<Impl> m_impl;
};

