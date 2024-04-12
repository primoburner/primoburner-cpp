cmake_minimum_required(VERSION 3.20)

# meta project that includes all other projects
project(primoburner-cpp)

# samples
add_subdirectory(samples samples)

#####
# This part is used only in published versions 
#####

# Operating System
string(TOLOWER ${CMAKE_SYSTEM_NAME} OS)

if(OS STREQUAL "windows")
    # copy primoburner
    set(SDK_LIB_PATH ${PROJECT_SOURCE_DIR}/sdk/lib/${PLATFORM})

    # Ninja
    set(BUILD_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/${PLATFORM})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_OUTPUT_PATH}
        COMMAND cmake -E copy_directory ${SDK_LIB_PATH} ${BUILD_OUTPUT_PATH}
    )

    # Visual Studio 2017 / 2019 / 2022
    if (CMAKE_GENERATOR MATCHES "Visual Studio.*20.*")
        set(BUILD_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/Debug)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_OUTPUT_PATH}
            COMMAND cmake -E copy_directory ${SDK_LIB_PATH} ${BUILD_OUTPUT_PATH}
        )

        set(BUILD_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/Release)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_OUTPUT_PATH}
            COMMAND cmake -E copy_directory ${SDK_LIB_PATH} ${BUILD_OUTPUT_PATH}
        )
    endif()    
endif()
