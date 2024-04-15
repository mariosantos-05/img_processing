cmake_minimum_required(VERSION 3.5)
project(my_project VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenCV 4.0.0 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(my_project src/main.cpp src/glad.c src/glad.h)

target_link_libraries(my_project glfw OpenGL::GL ${OpenCV_LIBS})
