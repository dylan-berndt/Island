
aux_source_directory(. GRAPHICS_SRC)

include_directories(../ ../external ../external/freetype/include)

link_directories(../external ../external/freetype)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

find_package(Freetype MODULE REQUIRED)

add_library(graphics ${GRAPHICS_SRC} ../imports.cpp ../external/glad.c ../external/glad/glad.h ../external/stb_image.h)
target_link_libraries(graphics PUBLIC glfw3 opengl32 Freetype::Freetype)
