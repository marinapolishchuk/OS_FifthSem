#pragma once

#include <memory>
#include <string>
#include <chrono>

class NamedPipe
{
private:
    struct Impl;
public:
    NamedPipe(std::unique_ptr<Impl>& impl);
    NamedPipe(NamedPipe&& other) = default;
    ~NamedPipe();

    NamedPipe& operator<<(const std::string& input);
    NamedPipe& operator>>(std::string& output);

    void CancelIO();

    void WaitForClient();

    bool IsValid() const;

    bool IsGood() const;
    void Clear();

public:
    static std::shared_ptr<NamedPipe> Create(const std::string& name);
    static std::shared_ptr<NamedPipe> Connect(const std::string& name);
private:
    std::unique_ptr<Impl> m_impl;
};

