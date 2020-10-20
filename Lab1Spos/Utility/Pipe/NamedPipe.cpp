#include "NamedPipe.hpp"

#include <Windows.h>
#include <iostream>

struct NamedPipe::Impl
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    DWORD lastError = ERROR_SUCCESS;
};

NamedPipe::~NamedPipe()
{
    if (m_impl)
    {
        CloseHandle(m_impl->handle);
    }
}

NamedPipe& NamedPipe::operator<<(const std::string& input)
{
    DWORD bytesWritten = 0;
    WriteFile(
        m_impl->handle,
        input.c_str(),
        input.size() * sizeof(char),
        &bytesWritten,
        NULL
    );

    return *this;
}

NamedPipe& NamedPipe::operator>>(std::string& output)
{
    static constexpr DWORD READ_BUFF_SIZE = 255;

    output.clear();

    DWORD nBytesRead = 0;
    do
    {
        char buff[READ_BUFF_SIZE];

        if (ReadFile(
            m_impl->handle,
            buff,
            READ_BUFF_SIZE * sizeof(char),
            &nBytesRead,
            NULL
        ))
        {
            output.append(buff, buff + nBytesRead);
        }
        m_impl->lastError = GetLastError();
    } while (m_impl->lastError == ERROR_MORE_DATA);

    return *this;
}

void NamedPipe::CancelIO()
{
    CancelIoEx(m_impl->handle, NULL);
}

void NamedPipe::WaitForClient()
{
    ConnectNamedPipe(m_impl->handle, NULL);
}

bool NamedPipe::IsValid() const
{
    return m_impl->handle != INVALID_HANDLE_VALUE;
}

bool NamedPipe::IsGood() const
{
    return m_impl->lastError == ERROR_SUCCESS;
}

void NamedPipe::Clear()
{
    m_impl->lastError = ERROR_SUCCESS;
}

std::shared_ptr<NamedPipe> NamedPipe::Create(const std::string& name)
{
    std::unique_ptr<Impl> impl = std::make_unique<Impl>();

    impl->handle = CreateNamedPipeA(
        name.data(),
        PIPE_ACCESS_DUPLEX ,
        PIPE_TYPE_MESSAGE |
        PIPE_READMODE_MESSAGE |
        PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        0,
        0,
        0,
        NULL
    );

    return std::make_shared<NamedPipe>(impl);
}

std::shared_ptr<NamedPipe> NamedPipe::Connect(const std::string& name)
{
    std::unique_ptr<Impl> impl = std::make_unique<Impl>();

    impl->handle = CreateFileA(
        name.data(),
        GENERIC_READ |
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    DWORD dwMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
    SetNamedPipeHandleState(
        impl->handle,
        &dwMode,
        NULL,
        NULL
    );

    return std::make_shared<NamedPipe>(impl);
}

NamedPipe::NamedPipe(std::unique_ptr<Impl>& impl)
{
    m_impl.swap(impl);
}
