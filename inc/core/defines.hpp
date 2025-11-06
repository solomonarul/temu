#pragma once
#ifndef CORE_DEFINES_HPP
#define CORE_DEFINES_HPP

#include <string>
#include <expected>

namespace Core
{
    template<typename T>
    using Result = std::expected<T, std::string>;
};

#endif