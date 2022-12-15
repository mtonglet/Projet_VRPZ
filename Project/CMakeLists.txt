project("Project")

set(SOURCES_EX_01 "Main.cpp" "source/shader.cpp" "source/camera.cpp" "source/Texture.cpp" "headers/Texture.h" "headers/camera.h" "headers/shader.h" "headers/object.h")

#These commands are there to specify the path to the folder containing the object and textures files as macro
#With these you can just use PATH_TO_OBJECTS and PATH_TO_TEXTURE in your c++ code and the compiler will replace it by the correct expression
add_compile_definitions(PATH_TO_OBJECTS="${CMAKE_CURRENT_SOURCE_DIR}/objects")
add_compile_definitions(PATH_TO_TEXTURE="${CMAKE_CURRENT_SOURCE_DIR}/textures")
add_compile_definitions(PATH_TO_MAIN="${CMAKE_CURRENT_SOURCE_DIR}")
add_compile_definitions(PATH_TO_SHADER="${CMAKE_CURRENT_SOURCE_DIR}/shaders")
add_compile_definitions(PATH_TO_HEADER="${CMAKE_CURRENT_SOURCE_DIR}/headers")
add_compile_definitions(PATH_TO_SOURCE="${CMAKE_CURRENT_SOURCE_DIR}/source")

add_executable(${PROJECT_NAME}_Main ${SOURCES_EX_01})
target_link_libraries(${PROJECT_NAME}_Main PUBLIC OpenGL::GL glfw glad)



