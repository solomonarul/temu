cmake_minimum_required(VERSION 3.10)

add_subdirectory(lib)

project(temu)

add_executable(
    ${PROJECT_NAME}
    "src/main.cpp"
    "src/app/handler.cpp"
    "src/bf/ir.cpp"
    "src/bf/runners/interpreter.cpp"
    "src/bf/runners/xbyak.cpp"
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        lini
        xbyak
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        inc/
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Werror -fno-exceptions>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Werror -fno-exceptions>
)

install(
    TARGETS
        ${PROJECT_NAME}
)