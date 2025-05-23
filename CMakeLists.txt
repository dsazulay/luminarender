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
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG 11.2.0
)
FetchContent_MakeAvailable(fmt)

# imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

add_library(imgui)
target_include_directories(
    imgui
    PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_sources(
    imgui
    PUBLIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(imgui glfw)

# imguizmo
FetchContent_Declare(
    imguizmo
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG master
)
FetchContent_MakeAvailable(imguizmo)

add_library(imguizmo)
target_include_directories(
    imguizmo
    PUBLIC
    ${imguizmo_SOURCE_DIR}
)

target_sources(
    imguizmo
    PUBLIC
    ${imguizmo_SOURCE_DIR}/imguizmo.cpp
)

target_link_libraries(imguizmo imgui)

# =============================================================

add_subdirectory(core)
add_subdirectory(editor)
# add_subdirectory(runtime)

