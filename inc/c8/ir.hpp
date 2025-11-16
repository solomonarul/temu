#pragma once

#include "../core/defines.hpp"

using namespace Core;

#include <vector>
#include <string>
#include <cstdint>

namespace C8
{
    struct Instruction
    {
        enum Type
        {
            C8_INSTRUCTION_NONE = 0,
            C8_INSTRUCTION_END,
            C8_INSTRUCTION_COUNT
        } type;

        ssize_t arg = -1, arg1 = -1;

        bool has_arguments(void);
        std::string args_to_string(void);
        static std::string_view type_to_string(Instruction::Type);
    };

    struct IR
    {
        Result<void> compile(std::vector<uint16_t>&);
        std::string dump(void);

        constexpr std::vector<Instruction>& get_code(void) { return code; }

    private:
        std::vector<Instruction> code;
    };
};