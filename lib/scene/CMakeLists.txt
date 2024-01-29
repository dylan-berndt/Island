
aux_source_directory(. SCENE_SRC)

include_directories(../ ../external)

add_library(scene ${SCENE_SRC} ../imports.cpp)