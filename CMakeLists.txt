cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(Minecraft-Daemon VERSION 0.1)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source directory
add_subdirectory(src)

# Add cppurses as a submodule
add_subdirectory(external/CPPurses)
