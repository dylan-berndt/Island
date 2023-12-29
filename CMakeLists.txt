cmake_minimum_required(VERSION 3.21)
project(Pipe)

include_directories(lib/external/glfw-3.3.9.bin.WIN64/include lib/external)

link_directories(lib/external/glfw-3.3.9.bin.WIN64/lib-mingw-w64 lib/external)

set(CMAKE_CXX_STANDARD 14)

add_executable(Pipe main.cpp lib/pipe.cpp lib/pipe.h lib/external/glad.c lib/external/glad/glad.h lib/back.h
        lib/shader.cpp lib/shader.h lib/textures.cpp lib/textures.h lib/vertex.cpp lib/vertex.h
        lib/external/stb_image.h)

target_link_libraries(Pipe glfw3 opengl32)
