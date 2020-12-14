cmake_minimum_required(VERSION 3.18)
project(Server LANGUAGES C)
include_directories(${PROJECT_SOURCE_DIR})
add_executable(server main.c)

target_link_libraries(server ${CMAKE_DL_LIBS} m gmp)
