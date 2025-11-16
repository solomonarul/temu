#pragma once

#include <cstring>
#include <memory>

#include "device.hpp"
#include "runners/f_all.hpp"

namespace BF
{
    struct State
    {
        void* userdata = nullptr;
        std::unique_ptr<IDevice> device;
        size_t memory[0xFFFF + 1];

        constexpr void reset(void) { memset(memory, 0, sizeof(memory)); }

    private:
        friend struct Runners::Interpreter;
        friend struct Runners::xbyak;
    };
};