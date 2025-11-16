#include "bf/devices/std.hpp"

using namespace BF::Devices;

#include <iostream>

STD::STD()
{
    std::cout.flush();

    handler.input = [](State&) -> size_t {
        char ch;
        if(!std::cin.get(ch))
            return std::char_traits<char>::eof();
        return static_cast<size_t>(ch);
    };

    handler.output = [](State&, size_t data) -> void {
        std::cout.put(static_cast<char>(data));
    };
}

STD::~STD()
{
    std::cout << '\n';
    // This also flushes.
}
