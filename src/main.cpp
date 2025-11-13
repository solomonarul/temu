#include <iostream>

#include "app/handler.hpp"

int main(int argc, char* argv[])
{
    TRY_OR(App::run(argc, argv), {
        std::cerr << TTY_ERROR(std::format("[EROR]: {}\n", _result.error()));
        return EXIT_FAILURE;
    });
    return EXIT_SUCCESS;
}