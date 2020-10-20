#include "Process.hpp"

#include <Utility/ExitCode.hpp>

#include <Windows.h>

struct Process::Impl
{
    HANDLE handle;
    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;
};

Process::~Process()
{
    if (m_impl)
    {
        Terminate();
        CloseHandle(m_impl->handle);
    }
}

std::shared_ptr<Process> Process::Create(const std::string &name, std::string cmd)
{
    std::unique_ptr<Impl> impl = std::make_unique<Impl>();

    impl->startupInfo = {0};
    impl->processInfo = {0};

    impl->startupInfo.cb = sizeof(impl->startupInfo);

    CreateProcessA(name.data(),
                   cmd.data(),
                   NULL,
                   NULL,
                   FALSE,
                   CREATE_SUSPENDED,
                   NULL,
                   NULL,
                   &(impl->startupInfo),
                   &(impl->processInfo));

    impl->handle = impl->processInfo.hProcess;

    return std::make_shared<Process>(impl);
}

void Process::Run()
{
    ResumeThread(m_impl->processInfo.hThread);
}

void Process::Join()
{
    DWORD exitCode = 0;
    GetExitCodeProcess(m_impl->handle, &exitCode);
    if (exitCode == STILL_ACTIVE)
    {
        WaitForSingleObject(m_impl->handle, INFINITE);
    }
}

void Process::Terminate()
{
    TerminateProcess(m_impl->handle, ExitCodes::PROCESS_TERMINATED);
}

Process::Process(std::unique_ptr<Impl> &impl)
{
    m_impl.swap(impl);
}
