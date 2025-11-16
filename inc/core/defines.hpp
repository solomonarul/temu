#pragma once

#include <string>
#include <format>
#include <expected>
#include <algorithm>

namespace Core
{
    template<typename T>
    using Result = std::expected<T, std::string>;

    static inline std::string& to_lower(std::string& s) noexcept {
        std::transform(s.begin(), s.end(), s.begin(),
                    [](unsigned char c){ return std::tolower(c); });
        return s;
    }
};

constexpr auto TTY = [](std::string_view text, int code) {
    return std::format("\033[{}m{}\033[0m", code, text);
};

constexpr auto TTY_ERROR   = [](std::string_view t){ return TTY(t, 31); };
constexpr auto TTY_GOOD = [](std::string_view t){ return TTY(t, 32); };
constexpr auto TTY_WARN  = [](std::string_view t){ return TTY(t, 33); };

#define ERROR(error) (std::unexpected(error))

#define ERROR_FMT(errorfmt, ...) (ERROR(std::format(errorfmt, __VA_ARGS__)))

#define IF_ERROR_RET(expr, error) \
    do { if(expr) return ERROR(error); } while(0)

#define IF_ERROR_FMT_RET(expr, errorfmt, ...) \
    do { if(expr) return ERROR_FMT(errorfmt, __VA_ARGS__); } while(0)

#define TRY(expr) \
    do { auto _r = (expr); if(!_r) return ERROR(_r.error()); } while(0)

#define TRY_OR(expr, block) \
    do { auto _result = (expr); if(!_result) block } while(0)
