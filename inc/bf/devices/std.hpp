#pragma once

#include "../device.hpp"

namespace BF::Devices
{
    struct STD : public IDevice
    {
        STD();
        ~STD();

        virtual constexpr EDevice get_type() override { return EDEVICE_STD; };
    };
};