cmake_minimum_required(VERSION 3.1.0)

project(gbmenu)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(gbmenu
    gb.cc
    cpu.cc
    ppu.cc
    gui.cc
    instruction.cc
)

target_compile_options(gbmenu PRIVATE -O2 -Wall)
target_link_libraries(gbmenu Qt5::Widgets Threads::Threads)
