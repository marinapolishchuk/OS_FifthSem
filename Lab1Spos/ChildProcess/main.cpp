#include "demofuncs.hpp"

#include <Utility/Pipe/NamedPipe.hpp>
#include <Utility/ExitCode.hpp>

#include <iostream>
#include <sstream>

enum class FUNC
{
    UNSET = -1,
    F,
    G
};

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        return ExitCodes::TO_FEW_ARGUMENTS;
    }

    FUNC funcToRun = FUNC::UNSET;

    switch (argv[1][0])
    {
    case 'f': funcToRun = FUNC::F; break;
    case 'g': funcToRun = FUNC::G; break;
    default: return ExitCodes::INVALID_SECOND_ARGUMENT;
    }

    auto pipe = NamedPipe::Connect(argv[0]);
    
    std::string str;
    *pipe >> str;

    std::istringstream oss(str);
    int case_nr = 0;
    
    if (oss >> case_nr)
    {
        if (case_nr >= std::size(spos::lab1::demo::op_group_traits<spos::lab1::demo::DOUBLE>::cases))
        {
            return ExitCodes::INVALID_CASE_NUMBER;
        }
        switch (funcToRun)
        {
        case FUNC::F: *pipe << std::to_string(spos::lab1::demo::f_func<spos::lab1::demo::DOUBLE>(case_nr)); break;
        case FUNC::G: *pipe << std::to_string(spos::lab1::demo::g_func<spos::lab1::demo::DOUBLE>(case_nr)); break;
        }
        return ExitCodes::SUCCESS;
    }
    return ExitCodes::INVALID_PIPE_INPUT;
}