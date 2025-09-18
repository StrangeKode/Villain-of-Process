cmake_minimum_required(VERSION 3.10)
project(VillainOfProcess)

set(CMAKE_CXX_STANDARD 17)

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/Process.cpp
    src/ProcessManager.cpp
    src/CpuMonitor.cpp
    src/Ui.cpp
)

# Incluye el directorio src para los headers
include_directories(src)

# Busca la librería ncurses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(VillainOfProcess ${SOURCES})

# Enlaza ncurses
target_link_libraries(VillainOfProcess ${CURSES_LIBRARIES})
