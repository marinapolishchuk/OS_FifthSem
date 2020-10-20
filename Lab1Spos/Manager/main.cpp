#include <Manager/Manager.h>
#include <Utility/ExitCode.hpp>

#include <future>
#include <iostream>
#include <sstream>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: manager.exe [path_to_child_process]\n";
        return ExitCodes::TO_FEW_ARGUMENTS;
    }

    Manager manager(argv[1]);

    int exitCode = manager.Run();

    if (exitCode == ExitCodes::SUCCESS)
    {
        std::cout << manager.GetComputationResult() << '\n';
    }

    return 0;
}
