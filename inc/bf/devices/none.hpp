#pragma once

#include "../device.hpp"

namespace BF::Devices
{
    struct None : public IDevice
    {
        virtual constexpr EDevice get_type() override { return EDEVICE_NONE; };
    };
};