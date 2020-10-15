#include <Utility/Pipe/NamedPipe.hpp>
#include <Utility/Process/Process.hpp>
#include <Utility/ExitCode.hpp>

#include <future>
#include <iostream>
#include <sstream>

std::future<double> WaitForProcessResult(const std::shared_ptr<Process>& process, const std::string& pipeName, int case_nr);

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: manager.exe [path_to_child_process]\n";
        return ExitCodes::TO_FEW_ARGUMENTS;
    }

    std::string fPipeName = "\\\\.\\pipe\\lab1spos-f_pipe";
    std::string gPipeName = "\\\\.\\pipe\\lab1spos-g_pipe";

    auto fProcess = Process::Create(argv[1], fPipeName + " f");
    auto gProcess = Process::Create(argv[1], gPipeName + " g");

    int case_nr = 0;

    std::cin >> case_nr;

    auto fProcessFuture = WaitForProcessResult(fProcess, fPipeName, case_nr);
    auto gProcessFuture = WaitForProcessResult(gProcess, gPipeName, case_nr);


    fProcess->Join();
    gProcess->Join();

    std::cout << "F process returned: " << fProcessFuture.get() << '\n';
    std::cout << "G process returned: " << gProcessFuture.get() << '\n';

    std::cout << "F process exit code: " << fProcess->GetExitCode() << '\n';
    std::cout << "G process exit code: " << gProcess->GetExitCode() << '\n';

    return 0;
}




std::future<double> WaitForProcessResult(const std::shared_ptr<Process>& process, const std::string& pipeName, int case_nr)
{
    auto processLambda =
        [=]() -> double
        {
            auto pipe = NamedPipe::Create(pipeName);
            auto pipeConnectionFuture = std::async(std::launch::async, [&]() { pipe->WaitForClient(); });

            process->Run();
            pipeConnectionFuture.wait();

            *pipe << std::to_string(case_nr);
            std::string result;
            *pipe >> result;

            std::istringstream iss(result);
            double d = 0;
            iss >> d;
            return d;
        };

    return std::async(std::launch::async, processLambda);
}
