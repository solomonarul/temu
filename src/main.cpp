#include <iostream>

#include "app/handler.hpp"

int main(int argc, char* argv[])
{
    TRY_OR(App::run(argc, argv), {
        std::cerr << "[EROR]: " << _result.error() << "\n";
        return EXIT_FAILURE;
    });
    return EXIT_SUCCESS;
}