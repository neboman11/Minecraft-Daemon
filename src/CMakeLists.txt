# Header file for version information
configure_file(versionConfig.h.in versionConfig.h)

# Add the source files
file(GLOB SOURCES "*.cpp")

# Add the executable
add_executable(Minecraft-Daemon ${SOURCES})

add_compile_definitions(CMAKE_BUILD)

option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message("-- Setting coverage flags")
    # Add required flags (GCC & LLVM/Clang)
    target_compile_options(Minecraft-Daemon PRIVATE
        -O0        # no optimization
        -g         # generate debug info
        --coverage # sets all required flags
    )
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
        target_link_options(Minecraft-Daemon PRIVATE --coverage)
    else()
        target_link_libraries(Minecraft-Daemon PRIVATE --coverage)
    endif()
endif(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

# Add support for pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Add libraries
target_link_libraries(Minecraft-Daemon Threads::Threads rudeconfig sqlite3)

# Include binary directory for the versionConfig.h file
target_include_directories(Minecraft-Daemon PUBLIC
                           "${PROJECT_BINARY_DIR}/src"
)

if (UNIX)
    # Add compiler options
    target_compile_options(Minecraft-Daemon PRIVATE -Wall -Wpedantic -Wextra)
endif (UNIX)

elif (WIN32)
    # Add compiler options
    target_compile_options(Minecraft-Daemon PRIVATE -Wall -Wextra)
endif (WIN32)
