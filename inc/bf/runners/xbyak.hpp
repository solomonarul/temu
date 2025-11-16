#pragma once

#include "../ir.hpp"
#include "../runner.hpp"

#define XBYAK_NO_EXCEPTION
#include <xbyak/xbyak.h>
#undef XBYAK_NO_EXCEPTION

#include <fstream>

namespace BF::Runners
{
    struct xbyak : public Xbyak::CodeGenerator, public IRunner
    {
        xbyak() : Xbyak::CodeGenerator(Xbyak::DEFAULT_MAX_CODE_SIZE, Xbyak::AutoGrow) {}

        constexpr std::string get_name() override { return "Xbyak JIT"; }

        Result<void> run(State&, IDeviceHandler*) override;
        Result<void> load_ir(IR&) override;
        void dump(std::ofstream&);
    };
};