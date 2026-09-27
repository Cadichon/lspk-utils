cmake_minimum_required(VERSION 3.23)

project(LSPKUtils)

option(FETCH_DEPS "Set to true to let CMake fetch third party dependencies" OFF)

if (FETCH_DEPS)
    include (FetchContent)

    # LZ4
    FetchContent_Declare(lz4
        GIT_REPOSITORY "https://github.com/lz4/lz4"
        GIT_TAG "v1.10.0"
        SOURCE_SUBDIR  build/cmake
    )
    FetchContent_MakeAvailable(lz4)
    add_library(lz4::lz4 ALIAS lz4)

    # argparse
    FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
        GIT_TAG "v3.2"
    )
    FetchContent_MakeAvailable(argparse)

else()
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(LZ4 REQUIRED IMPORTED_TARGET liblz4)
    add_library(lz4::lz4 ALIAS PkgConfig::LZ4)

    find_package(argparse CONFIG REQUIRED)
endif()

add_executable(lspk-utils)

target_compile_features(lspk-utils
    PRIVATE
        cxx_std_23
)

target_sources(lspk-utils
    PRIVATE
        src/main.cpp
        src/commands/dump.cpp
        src/commands/pak.cpp
        src/commands/unpak.cpp
)

if (WIN32)
    target_sources(lspk-utils
        PRIVATE
            src/memory_map/file_win.cpp
    )
else()
    target_sources(lspk-utils
        PRIVATE
            src/memory_map/file.cpp
    )
endif()

target_include_directories(lspk-utils
    PRIVATE
        src
)

target_link_libraries(lspk-utils
    PRIVATE
        lz4::lz4
        argparse::argparse
)

if(MINGW)
    # Linking error on mingw
    target_link_libraries(lspk-utils
        PRIVATE
            stdc++exp
    )
endif()
