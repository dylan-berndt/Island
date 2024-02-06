
aux_source_directory(. PIPE_SRC)

include_directories(external graphics scene external/glfw-3.3.9.bin.WIN64/include external/freetype/include)
link_directories(lib/external/glfw-3.3.9.bin.WIN64/lib-mingw-w64)

add_subdirectory(graphics)
add_subdirectory(scene)

add_library(Pipe ${PIPE_SRC} external/glad.c external/glad/glad.h external/stb_image.h)
target_link_libraries(Pipe PUBLIC graphics scene)