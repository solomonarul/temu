#pragma once
#ifndef APP_HANDLER_HPP
#define APP_HANDLER_HPP

#include "../core/defines.hpp"

using namespace Core;

namespace App
{
    Result<void> run(int argc, char* argv[]);
};

#endif