cmake_minimum_required(VERSION 3.1)

project(template C)

add_compile_options(/Od)
include_directories()

add_executable(${PROJECT_NAME} WIN32 src/main.c)
