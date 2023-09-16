cmake_minimum_required(VERSION 3.26)

IF (CMAKE_HOST_WIN32)
set(CMAKE_C_COMPILER "C:\\mingw64\\bin\\mingw64")
ENDIF()

project(pacman_raylib C)
# Generate compile_commands.json

# Dependencies
IF(UNIX)
set(RAYLIB_VERSION 4.5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
include(FetchContent)
FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
    endif()
    
ENDIF()
# Our Project
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} main.c)
#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib)