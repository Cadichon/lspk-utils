cmake_minimum_required(VERSION 3.23)

project(pak)

find_package(lz4 CONFIG REQUIRED)

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
)

if(MINGW)
    # Linking error on mingw
    target_link_libraries(pak
        PRIVATE
            stdc++exp
    )
endif()
