cmake_minimum_required(VERSION 3.20)

project(bluray-data)
set (target bluray-data)

add_executable(${target})

# Operating System
string(TOLOWER ${CMAKE_SYSTEM_NAME} OS)

# output
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../../bin/${PLATFORM})

if (CMAKE_GENERATOR MATCHES "Visual Studio.*20.*")
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../../bin)
endif()

# debug definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${target} PUBLIC  _DEBUG _UNICODE UNICODE PRIMO_UTF16 )
endif()

# release definitions
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${target} PUBLIC NDEBUG  _UNICODE UNICODE PRIMO_UTF16)
endif()

# Windows
if(OS STREQUAL "windows")
    # add /MDd or /MD compiler option 
    set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # add /MTd or /MT compiler option 
    # set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    # enable C++ 17 standard features
    target_compile_features(${target} PRIVATE cxx_std_17)

    # assume extern "C" functions do not throw. 
    # see: https://docs.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model?view=vs-2019
    target_compile_options(${target} PRIVATE /EHsc)

    # x64 compile options
    if (PLATFORM STREQUAL "x64" OR PLATFORM STREQUAL "arm64") 
        target_compile_definitions(${target} PRIVATE WIN64)
    endif()

    # x86 compile options
    if (PLATFORM STREQUAL "x86") 
        target_compile_definitions(${target} PRIVATE WIN32)
    endif()

    # debug compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PRIVATE /Zi /Od)
    endif()

    # release compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target} PRIVATE /O2)
    endif()     
endif()

# include dirs
target_include_directories(${target}
    PUBLIC
        ../../../sdk/include
)

# sources
file(GLOB source "./*.cpp")

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
if(OS STREQUAL "windows")
    target_link_libraries(
        ${target}
        
        # primo-burner
        PrimoBurner64.dll

        # os
        Shlwapi 
    )
endif()

# third party libraries via vcpkg

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