#pragma once
#ifndef BF_INTERPRETER_HPP
#define BF_INTERPRETER_HPP

#include "../runner.hpp"

namespace BF::Runners
{
    struct Interpreter : IRunner
    {
        Result<void> run(State&) override;
        Result<void> load_ir(IR&) override;

        constexpr std::string get_name(void) override { return "Interpreter"; }

    private:
        std::vector<Instruction> code;
    };
};

#endif