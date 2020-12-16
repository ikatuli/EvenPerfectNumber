cmake_minimum_required(VERSION 3.18)
project(client LANGUAGES C)
include_directories(${PROJECT_SOURCE_DIR})
add_executable(client main.c)

# Использует прогу PkgConfig для обнаружения заголовками GTK+
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

# Подрубаем директории с gtk либами и заголовками.
INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})

# Добавим флаги компиляции.
ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})

target_link_libraries(client ${GTK3_LIBRARIES} pthread)

