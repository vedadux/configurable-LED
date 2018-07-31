cmake_minimum_required(VERSION 3.7.2)
project(configurable_LED)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -g)
include(${CMAKE_CURRENT_SOURCE_DIR}/AssureOutOfSourceBuilds.txt)
AssureOutOfSourceBuilds()

set(SOURCE_FILES
    led.c
    led.h)

add_executable(led-64 ${SOURCE_FILES})
target_compile_definitions(led-64 PRIVATE LED=64)

add_executable(led-128 ${SOURCE_FILES})
target_compile_definitions(led-128 PRIVATE LED=128)