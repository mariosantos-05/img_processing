cmake_minimum_required(VERSION 3.5)
project(Paper_redo VERSION 0.1.0)

# Find OpenCV
find_package(OpenCV 4.0.0 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Find GTK+ 3.x
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})

# Set WITH_GTK_3 option for OpenCV
set(WITH_GTK_3 ON)

# Add the executable
add_executable(paper src/main4.cpp)

# Link against OpenCV and GTK
target_link_libraries(paper ${OpenCV_LIBS} ${GTK3_LIBRARIES})
