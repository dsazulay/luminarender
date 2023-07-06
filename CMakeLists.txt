cmake_minimum_required(VERSION 3.21)
project(LuminaRender)

set(CMAKE_CXX_STANDARD 20)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})

# =============================================================

# Dependencies

# Glad
message(STATUS "Installing glad")
add_subdirectory(external/glad)
set_property(TARGET Glad PROPERTY FOLDER "Dependencies")

# ImGui
message(STATUS "Installing imgui")
add_subdirectory(external/imgui)
set_property(TARGET ImGui PROPERTY FOLDER "Dependencies")

# ImGui
message(STATUS "Installing imguizmo")
add_subdirectory(external/imguizmo)
set_property(TARGET ImGuizmo PROPERTY FOLDER "Dependencies")

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" FORCE)
message(STATUS "Installing glfw via submodule")
add_subdirectory(external/glfw)
set_property(TARGET glfw PROPERTY FOLDER "Dependencies")

# glm
message(STATUS "Installing glm via submodule")
set(BUILD_STATIC_LIBS ON)
add_subdirectory(external/glm)
set_property(TARGET glm_static PROPERTY FOLDER "Dependencies")

# fmt
message(STATUS "Installing fmt via submodule")
add_subdirectory(external/fmt)
set_property(TARGET fmt PROPERTY FOLDER "Dependencies")

# =============================================================

# Sources
file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_executable(${PROJECT_NAME} ${FILE_SOURCES})

target_link_directories(${PROJECT_NAME} PUBLIC lib/assimp)

target_link_libraries(${PROJECT_NAME}
    Glad
    ImGui
    ImGuizmo
    glfw
    glm_static
    fmt
    assimp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC external/glfw
    PUBLIC external/glm
    PUBLIC external/fmt
    PUBLIC include
)


