cmake_minimum_required(VERSION 3.23)

project(pak)

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

    # args
    set(ARGS_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
    set(ARGS_BUILD_UNITTESTS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(args
        GIT_REPOSITORY "https://github.com/taywee/args"
        GIT_TAG "6.6.0"
    )
    FetchContent_MakeAvailable(args)

else()
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(LZ4 REQUIRED IMPORTED_TARGET liblz4)
    add_library(lz4::lz4 ALIAS PkgConfig::LZ4)

    find_package(args CONFIG REQUIRED)
endif()

add_executable(pak)

target_compile_features(pak
    PRIVATE
        cxx_std_23
)

target_sources(pak
    PRIVATE
        src/main.cpp
)

if (WIN32)
    target_sources(pak
        PRIVATE
            src/memory_map/file_win.cpp
    )
else()
    target_sources(pak
        PRIVATE
            src/memory_map/file.cpp
    )
endif()

target_include_directories(pak
    PRIVATE
        src
)

target_link_libraries(pak
    PRIVATE
        lz4::lz4
        taywee::args
)

if(MINGW)
    # Linking error on mingw
    target_link_libraries(pak
        PRIVATE
            stdc++exp
    )
endif()
