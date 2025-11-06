#include <iostream>

#include "app/handler.hpp"

int main(int argc, char* argv[])
{
    auto result = App::run(argc, argv);
    if(!result)
    {
        std::cerr << "[EROR]: " << result.error() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}