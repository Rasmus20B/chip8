cmake_minimum_required(VERSION 3.5.1)  # CMake version check
project(idtd C)               # Create project "simple_example"
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -march=native")

# Add main.cpp file of project root directory as source file
set(ASM_SOURCE_FILES 
  src/asm.c 
  src/asm.h
)
set(EMU_SOURCE_FILES
  src/cpu.c
  src/cpu.h
)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Add executable target with source files listed in SOURCE_FILES variable
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
# include_directories("${CMAKE_SOURCE_DIR}/dependencies/lodge/src")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(chip8 ${EMU_SOURCE_FILES})
add_executable(c8asm ${ASM_SOURCE_FILES})
target_link_libraries(chip8 ${SDL2_LIBRARIES})
