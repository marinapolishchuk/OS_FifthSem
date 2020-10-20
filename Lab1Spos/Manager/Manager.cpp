#include "Manager.h"

#include <Utility/ExitCode.hpp>

#include <future>
#include <sstream>
#include <iostream>

#include <windows.h>

std::future<std::pair<double, bool>> WaitForProcessResult(const std::shared_ptr<Process>& process, const std::shared_ptr<NamedPipe>& pipe, int case_nr);

Manager::Manager(const std::string& cmd)
{
    using namespace std::string_literals;

    constexpr const char* fPipeName = "\\\\.\\pipe\\lab1spos-f_pipe";
    constexpr const char* gPipeName = "\\\\.\\pipe\\lab1spos-g_pipe";

    fProcess = Process::Create(cmd, fPipeName + " f"s);
    gProcess = Process::Create(cmd, gPipeName + " g"s);

    fPipe = NamedPipe::Create(fPipeName);
    gPipe = NamedPipe::Create(gPipeName);
}

int Manager::Run()
{
    std::atomic_bool isDone = false;
    std::thread escCancelationThread([&]()
                                     {
                                         using namespace std::chrono_literals;
                                         while (!isDone)
                                         {
                                             if (GetAsyncKeyState(VK_ESCAPE) == 0)
                                             {
                                                 std::this_thread::sleep_for(10ms);
                                             }
                                             else
                                             {
                                                 fProcess->Terminate();
                                                 gProcess->Terminate();
                                                 std::exit(ExitCodes::ESC_PRESSED);
                                             }
                                         }
                                     });

    int case_nr = 0;
    std::cin >> case_nr;

    auto fProcessFuture = WaitForProcessResult(fProcess, fPipe, case_nr);
    auto gProcessFuture = WaitForProcessResult(gProcess, gPipe, case_nr);

    std::pair<double, bool> fProcessResult;
    std::pair<double, bool> gProcessResult;

    auto fShortCircuitEvaluator = std::async(std::launch::async,
                                             [&]()
                                             {
                                                 fProcessResult = fProcessFuture.get();
                                                 if (fProcessResult.first == 0 && fProcessResult.second == true)
                                                 {
                                                     gPipe->CancelIO();
                                                     gProcess->Terminate();
                                                 }
                                             });

    auto gShortCircuitEvaluator = std::async(std::launch::async,
                                             [&]()
                                             {
                                                 gProcessResult = gProcessFuture.get();
                                                 if (gProcessResult.first == 0 && gProcessResult.second == true)
                                                 {
                                                     fPipe->CancelIO();
                                                     fProcess->Terminate();
                                                 }
                                             });

    fShortCircuitEvaluator.wait();
    gShortCircuitEvaluator.wait();

    if ((fProcessResult.first == 0 && fProcessResult.second == true) || (gProcessResult.first == 0 && gProcessResult.second == true))
    {
        std::cout << "Zero result!\n";
        isDone = true;
        escCancelationThread.join();
        return ExitCodes::ZERO_RESULT;
    }

    result = fProcessResult.first * gProcessResult.first;
    isDone = true;
    escCancelationThread.join();
    return ExitCodes::SUCCESS;
}

double Manager::GetComputationResult()
{
    return result;
}


std::future<std::pair<double, bool>> WaitForProcessResult(const std::shared_ptr<Process>& process, const std::shared_ptr<NamedPipe>& pipe, int case_nr)
{
    using namespace std::chrono_literals;

    auto processLambda =
        [=]() -> std::pair<double, bool>
    {
        auto pipeConnectionFuture = std::async(std::launch::async, [&]() { pipe->WaitForClient(); });

        process->Run();
        pipeConnectionFuture.wait();

        *pipe << std::to_string(case_nr);
        std::string result;
        *pipe >> result;
        std::istringstream iss(result);
        double d = 0;
        iss >> d;
        return { d, pipe->IsGood() };
    };

    return std::async(std::launch::async, processLambda);
}
