cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64") # temporarily disabling x86_64 just to see if that's the whole problem
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(gay-wave-trail VERSION 1.0.0)
add_subdirectory(tools/corrosion)

file(
  GLOB_RECURSE
  SOURCES
  src/*.cpp
  src/utils/*.cpp
  src/trail_customization/*.cpp
  src/settings/*.cpp
  src/ui/*.cpp
  src/types/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include src)

if(NOT DEFINED ENV{GEODE_SDK})
  message(
    FATAL_ERROR
      "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode"
  )
else()
  message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()


corrosion_import_crate(MANIFEST_PATH tools/rust/colortools/Cargo.toml)
corrosion_experimental_cbindgen(
  TARGET colortools
  HEADER_NAME "colortools.hpp"
)

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

target_link_libraries(${PROJECT_NAME} colortools)
setup_geode_mod(${PROJECT_NAME})
