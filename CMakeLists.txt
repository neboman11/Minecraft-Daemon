cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(Minecraft-Daemon VERSION 0.1)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Header file for version information
configure_file(versionConfig.h.in versionConfig.h)

# Add the source files
file(GLOB SOURCES "*.cpp")

# Add the executable
add_executable(Minecraft-Daemon ${SOURCES})

add_compile_definitions(CMAKE_BUILD)

# Add cppurses as a submodule
add_subdirectory(external/CPPurses)

# Add support for pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Add libraries
target_link_libraries(Minecraft-Daemon Threads::Threads rudeconfig cppurses)

# Include binary directory for the versionConfig.h file
target_include_directories(Minecraft-Daemon PUBLIC
                           "${PROJECT_BINARY_DIR}"
)

# Add compiler options
target_compile_options(Minecraft-Daemon PRIVATE -Wall -Wextra -Wpedantic -Werror)
