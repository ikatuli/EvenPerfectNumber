cmake_minimum_required(VERSION 3.18)
project(client LANGUAGES C)
include_directories(${PROJECT_SOURCE_DIR})
add_executable(client main.c)

target_link_libraries(client ${CMAKE_DL_LIBS})
