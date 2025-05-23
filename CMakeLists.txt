cmake_minimum_required(VERSION 3.28)
include(FetchContent)

project(LuminaRender)

set(CMAKE_CXX_STANDARD 20)

#
# Dependencies
#

# glad
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
)
FetchContent_MakeAvailable(glad)
add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)
glad_add_library(glad REPRODUCIBLE EXCLUDE_FROM_ALL LOADER API gl:core=4.1)

# glfw
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    FIND_PACKAGE_ARGS NAMES glfw3
)
FetchContent_MakeAvailable(glfw)

# glm
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(glm)

# assimp
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.4.3
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(assimp)

# fmt
message(STATUS "Installing fmt via submodule")
add_subdirectory(external/fmt)
set_property(TARGET fmt PROPERTY FOLDER "Dependencies")

# ImGuizmo
message(STATUS "Installing imguizmo")
add_subdirectory(external/imguizmo)

# =============================================================

add_subdirectory(core)
add_subdirectory(editor)
# add_subdirectory(runtime)

