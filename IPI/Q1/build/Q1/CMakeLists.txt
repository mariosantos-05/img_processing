cmake_minimum_required(VERSION 3.5)
project(ler_YUV)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Add executable
add_executable(ler_YUV main.cpp)

# Link OpenCV libraries
target_link_libraries(ler_YUV ${OpenCV_LIBS})


