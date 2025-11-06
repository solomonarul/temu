cmake_minimum_required(VERSION 3.10)

project(lini)

add_library(
    ${PROJECT_NAME}
        "src/file.cpp"   
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        "inc/lini"
    PUBLIC
        "inc/"   
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON
)