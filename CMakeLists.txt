cmake_minimum_required(VERSION 3.20)
project(LuqRenderer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── Source files ────────────────────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Mesh.cpp
    src/Renderer.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# ─── Dependencies ─────────────────────────────────────────────────────────────
# Requires GLFW, GLAD, and GLM to be installed or available via package manager.
#
# macOS (Homebrew):  brew install glfw glm
# Ubuntu/Debian:     sudo apt install libglfw3-dev libglm-dev
#
# For GLAD: generate a loader at https://glad.dav1d.de/ (OpenGL 3.3 core)
# and add glad.c to your project, or use the CMake integration from vcpkg.

find_package(OpenGL REQUIRED)

# GLFW
find_package(glfw3 3.3 REQUIRED)

# GLM (header-only)
find_package(glm REQUIRED)

# GLAD — expected at third_party/glad/
# If using vcpkg or system package, adjust accordingly.
target_include_directories(${PROJECT_NAME} PRIVATE
    src
    third_party/glad/include
)

target_sources(${PROJECT_NAME} PRIVATE
    third_party/glad/src/glad.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glm::glm
)

# ─── Compiler warnings ───────────────────────────────────────────────────────
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ─── Copy shaders to build directory ─────────────────────────────────────────
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    COMMENT "Copying shaders to build directory"
)
