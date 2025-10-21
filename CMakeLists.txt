cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(GayWaveTrail VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE SOURCES src/**.cpp)

include_directories(AFTER ${PROJECT_NAME} include)

add_compile_options(
	$<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Wpedantic -Wno-reserved-id-macro -Wno-dollar-in-identifier-extension>
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

if(NOT DEFINED ENV{GEODE_SDK})
    message(
        FATAL_ERROR
        "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode"
    )
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})
