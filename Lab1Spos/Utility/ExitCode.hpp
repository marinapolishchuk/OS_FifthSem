#pragma once

namespace ExitCodes
{
    constexpr int SUCCESS = 0;
    constexpr int TO_FEW_ARGUMENTS = -1;
    constexpr int INVALID_SECOND_ARGUMENT = -2;
    constexpr int INVALID_PIPE_INPUT = -3;
    constexpr int PROCESS_TERMINATED = -4;
    constexpr int ESC_PRESSED = -5;
    constexpr int ZERO_RESULT = -6;
    constexpr int INVALID_CASE_NUMBER = -7;
}