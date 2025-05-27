cmake_minimum_required(VERSION 3.16)
project(chip8_emulator_core VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(chip8_core STATIC)

# Add source files - use GLOB_RECURSE to find files in subdirectories
file(GLOB_RECURSE CHIP8_CORE_SOURCES src/*.cpp)
file(GLOB_RECURSE CHIP8_CORE_HEADERS include/*.h include/*.hpp)

set_target_properties(chip8_core PROPERTIES
    PUBLIC_HEADER "${CHIP8_CORE_HEADERS}"
)

# Target sources
target_sources(chip8_core PRIVATE ${CHIP8_CORE_SOURCES} ${CHIP8_CORE_HEADERS})

target_include_directories(chip8_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
