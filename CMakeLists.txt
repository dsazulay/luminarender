cmake_minimum_required(VERSION 3.21)
project(InteractiveGraphics)

set(CMAKE_CXX_STANDARD 20)

include_directories("include")
include_directories("imgui")
include_directories("/opt/homebrew/include")

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})

add_executable(InteractiveGraphics
        main.cpp
        glad.c
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_opengl3.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        window.cpp window.h
        imgui_renderer.cpp imgui_renderer.h
        primitives.h
        camera.cpp camera.h
        importer.cpp importer.h
        include/stb_image.h
        assets/shader.cpp assets/shader.h
        uniform_buffer_object.cpp uniform_buffer_object.h
        frame_buffer.cpp frame_buffer.h
        assets/material.cpp assets/material.h
        renderer.cpp renderer.h
        components/transform.cpp components/transform.h
        assets/mesh.cpp assets/mesh.h
        components/light.cpp components/light.h
        entity_factory.cpp entity_factory.h
        components/icomponent.h
        entity.cpp entity.h
        scene.cpp scene.h
        log.h
        asset_library.cpp asset_library.h
        components/mesh_renderer.cpp components/mesh_renderer.h
        assets/model.cpp assets/model.h
        events/event.h
        events/dispatcher.cpp events/dispatcher.h
        assets/texture.cpp assets/texture.h
        application.cpp application.h
        pch.h
        samples/sample_resources.h
        samples/unlit_scene.cpp samples/unlit_scene.h
        samples/lighting_skybox_scene.cpp samples/lighting_skybox_scene.h samples/pbr_scene.cpp samples/pbr_scene.h irradiance_map_factory.cpp irradiance_map_factory.h renderer/irenderable.h renderer/shadow_pass.cpp renderer/shadow_pass.h renderer/render_system.cpp renderer/render_system.h renderer/forward_pass.cpp renderer/forward_pass.h)

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(InteractiveGraphics PUBLIC glfw)

target_link_directories(InteractiveGraphics PUBLIC lib/assimp)
target_link_libraries(InteractiveGraphics PUBLIC assimp)

#target_precompile_headers(InteractiveGraphics PUBLIC pch.h)
