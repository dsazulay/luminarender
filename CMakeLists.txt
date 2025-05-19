cmake_minimum_required(VERSION 3.21)
project(LuminaRender)

set(CMAKE_CXX_STANDARD 20)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/editor)


add_subdirectory(editor)
