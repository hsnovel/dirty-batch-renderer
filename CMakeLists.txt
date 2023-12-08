cmake_minimum_required(VERSION 3.1)

project(template C)

add_compile_options(/Od)

add_executable(${PROJECT_NAME} WIN32 src/main.c src/glad.c)

# include glad.h
include_directories(external)

add_subdirectory(external/glfw)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include)

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include)

target_link_libraries(${PROJECT_NAME} glfw)
