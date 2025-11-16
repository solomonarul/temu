#pragma once

#include "../runner.hpp"

namespace BF::Runners
{
    struct Interpreter : public IRunner
    {
        Result<void> run(State&, IDeviceHandler*) override;
        Result<void> load_ir(IR&) override;

        constexpr std::string get_name(void) override { return "Interpreter"; }

    private:
        std::vector<Instruction> code;
    };
};