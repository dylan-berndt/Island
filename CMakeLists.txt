cmake_minimum_required(VERSION 3.21)

set(PROJECT_NAME Island)

project(${PROJECT_NAME})

add_subdirectory(lib)

include_directories(lib/external/glfw-3.3.9.bin.WIN64/include lib/external lib/graphics lib/scene)

link_directories(lib/external/glfw-3.3.9.bin.WIN64/lib-mingw-w64 lib/external)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -lpthread")

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} Pipe)
