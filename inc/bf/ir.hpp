#pragma once
#ifndef BF_IR_HPP
#define BF_IR_HPP

#include "../core/defines.hpp"

using namespace Core;

#include <vector>
#include <string>

#include "runners/f_all.hpp"

namespace BF
{
    struct Instruction
    {
        enum Type
        {
            BF_INSTRUCTION_NONE = 0,
            BF_INSTRUCTION_ADD, // + -
            BF_INSTRUCTION_MOV, // < >
            BF_INSTRUCTION_JMP, // [ ]
            BF_INSTRUCTION_INP, //  ,
            BF_INSTRUCTION_OUT, //  .
            BF_INSTRUCTION_CLR, // [-]
            BF_INSTRUCTION_END,
            BF_INSTRUCTION_COUNT
        } type;

        ssize_t arg;

        bool has_arguments(void);
        std::string args_to_string(void);
        static std::string_view type_to_string(Instruction::Type);
    };

    struct IR
    {
        struct CompileFlags {
            bool input, output;
        };

        Result<void> compile(std::string&, CompileFlags = {.input = true, .output = true});
        std::string dump(void);

        constexpr std::vector<Instruction>& get_code(void) { return code; }

    private:
        std::vector<Instruction> code;

        void optimize(void);

        friend struct BF::Runners::Interpreter;
        friend struct BF::Runners::xbyak;
    };
};

#endif