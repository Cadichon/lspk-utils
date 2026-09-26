cmake_minimum_required(VERSION 3.23)

project(pak)

add_executable(pak)

target_compile_features(pak
    PRIVATE
        cxx_std_23
)

target_sources(pak
    PRIVATE
        src/main.cpp
)

target_include_directories(pak
    PRIVATE
        src
)
