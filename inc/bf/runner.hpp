#pragma once

#include "../core/defines.hpp"
#include "ir.hpp"
#include "state.hpp"

using namespace Core;

namespace BF
{
    struct IRunner
    {
        virtual ~IRunner() = default;
        virtual Result<void> run(State&, IDeviceHandler*) = 0;
        virtual Result<void> load_ir(IR&) = 0;
        virtual constexpr std::string get_name(void) = 0;
    };
};