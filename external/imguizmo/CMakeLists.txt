cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
cmake_policy(VERSION 3.6)
project(ImGuizmo
        VERSION 1.0.0.0
        LANGUAGES C CXX
)


file(GLOB_RECURSE IMGUIZMO_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)


add_library(
  ${PROJECT_NAME}
  "${IMGUIZMO_SOURCES}"
)

target_link_libraries(
    ${PROJECT_NAME}
    ImGui
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC external/imgui
)

add_dependencies(
    ${PROJECT_NAME}
    ImGui
)


target_include_directories(
  ImGuizmo
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
