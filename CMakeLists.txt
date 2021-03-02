cmake_minimum_required(VERSION 3.1)

project(animation-sample)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(sources 
    glad.c 
    main.cpp 
    Application.cpp
    Attribute.cpp
    Draw.cpp
    IndexBuffer.cpp
    Mat4.cpp
    Shader.cpp
)

add_executable(animation-sample ${sources})

target_link_libraries(animation-sample ${OPENGL_LIBRARIES} glfw)
