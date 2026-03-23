cmake_minimum_required(VERSION 3.20)
project(pak VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(pak pak.c pak.h)
target_include_directories(pak PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(pak PRIVATE _DEFAULT_SOURCE)
