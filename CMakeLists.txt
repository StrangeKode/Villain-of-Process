cmake_minimum_required(VERSION 3.10)
project(minihtop_plus VERSION 1.0 LANGUAGES CXX)

# Configuración del compilador
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra")

# Ejecutable
add_executable(vop vop_cpp.cpp)

# Link con ncurses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(vop ${CURSES_LIBRARIES})
