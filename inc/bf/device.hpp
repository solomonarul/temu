#pragma once

#include <cstddef>

namespace BF
{
    struct State;

    using F_in = size_t(*)(State&);
    using F_out = void(*)(State&, size_t);

    enum EDevice
    {
        EDEVICE_STD = 0,
        EDEVICE_NONE,
        EDEVICE_INVALID,
        EDEVICE_COUNT
    };

    struct IDeviceHandler
    {
        F_in input = nullptr;
        F_out output = nullptr;
    };

    struct IDevice
    {
        IDeviceHandler handler;

        virtual ~IDevice() = default;
        virtual constexpr EDevice get_type() { return EDEVICE_INVALID; };
    };
};
