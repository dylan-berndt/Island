
aux_source_directory(. GRAPHICS_SRC)

include_directories(../ ../external)

add_library(graphics ${GRAPHICS_SRC} ../imports.cpp ../external/glad.c ../external/glad/glad.h ../external/stb_image.h)
target_link_libraries(graphics PUBLIC glfw3 opengl32)