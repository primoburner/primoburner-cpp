cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "$ENV${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(bluray-data)
set (target bluray-data)

add_executable(${target})

# Operating System
string(TOLOWER ${CMAKE_SYSTEM_NAME} OS)

# output
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../../bin/${PLATFORM})

if (CMAKE_GENERATOR STREQUAL "Xcode")
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../../bin)
endif()

# debug definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${target} PUBLIC  _DEBUG _UNICODE)
endif()

# release definitions
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${target} PUBLIC NDEBUG _UNICODE)
endif()

# macOS
if(OS STREQUAL "darwin") 
    # common compile options
    target_compile_options(${target} PRIVATE -std=c++17 -stdlib=libc++)

    # x64 compile options
    if (PLATFORM STREQUAL "x64") 
        target_compile_options(${target} PRIVATE -m64 -fPIC)
    endif()

    # x86 compile options
    if (PLATFORM STREQUAL "x86") 
    target_compile_options(${target} PRIVATE -m32)
    endif()

    # debug compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PRIVATE -g)
    endif()

    # release compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target} PRIVATE -Os)
    endif()
endif()

# include dirs
target_include_directories(${target}
    PUBLIC
        ../../../sdk/include
)

# sources
file(GLOB source "./*.cpp" "./*.mm")

target_sources(${target}
PRIVATE
    ${source} 
)

# lib dirs
target_link_directories(${target}
PRIVATE
    # avblocks
    ${PROJECT_SOURCE_DIR}/../../../sdk/lib/${PLATFORM}
)

# libs
if (OS STREQUAL "darwin")
    target_link_libraries(
        ${target}

        # primo-burner
        libPrimoBurner.dylib

        # os
        "-framework CoreFoundation"
        "-framework AppKit"  
    )
endif()

# third party libraries installed via vcpkg

## fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${target} fmt::fmt)

## argparse
find_package(argparse CONFIG REQUIRED)
target_link_libraries(${target} argparse::argparse)

## tabulate
find_package(tabulate CONFIG REQUIRED)
target_link_libraries(${target} tabulate::tabulate)

## indicators
find_package(indicators CONFIG REQUIRED)
target_link_libraries(${target} indicators::indicators)