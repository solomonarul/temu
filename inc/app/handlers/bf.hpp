#pragma once
#ifndef APP_BF_HANDLER_HPP
#define APP_BF_HANDLER_HPP

#include "core/defines.hpp"

using namespace Core;

#include <lini/file.hpp>

Result<void> handle_bf_emulation(LINI::File& ini_file);

#endif