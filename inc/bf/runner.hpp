#pragma once
#ifndef BF_RUNNER_HPP
#define BF_RUNNER_HPP

#include "../core/defines.hpp"
#include "ir.hpp"
#include "state.hpp"

using namespace Core;

namespace BF
{
    struct IRunner
    {
        virtual Result<void> load_ir(IR&) = 0;
        virtual Result<void> run(State&) = 0;

        virtual constexpr std::string get_name(void) = 0;
    };
};

#endif