#pragma once
#ifndef BF_STATE_HPP
#define BF_STATE_HPP

#include <cstring>
#include <cstddef>

#include "runners/f_all.hpp"

namespace BF
{
    struct State
    {
        void* userdata = nullptr;
        size_t (*f_in)(State&) = nullptr;
        void (*f_out)(State&, size_t ch) = nullptr;
        size_t memory[0xFFFF + 1];

        constexpr void reset(void) { memset(memory, 0, sizeof(memory)); }

    private:
        friend struct Runners::Interpreter;
        friend struct Runners::xbyak;
    };
};

#endif