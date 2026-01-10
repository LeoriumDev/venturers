cmake_minimum_required(VERSION 4.0)
project(venturers LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

add_subdirectory(external/glfw)
add_subdirectory(external/glm)

add_library(glad STATIC
        external/glad/src/gl.c
)

target_include_directories(glad PUBLIC
        ${PROJECT_SOURCE_DIR}/external/glad/include
)

add_executable(venturers
        src/main.cpp
        src/camera/camera.hpp)

target_include_directories(venturers PRIVATE
        ${PROJECT_SOURCE_DIR}/external/stb
)

target_link_libraries(venturers
        PRIVATE
        glfw
        glad
        glm::glm
        OpenGL::GL
)

file(GLOB CONFIGURE_DEPENDS SHADER_FILES
        "${CMAKE_SOURCE_DIR}/shaders/*.vert"
        "${CMAKE_SOURCE_DIR}/shaders/*.frag"
)

file(GLOB CONFIGURE_DEPENDS ASSET_FILES
        "${CMAKE_SOURCE_DIR}/assets/**"
)

add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:venturers>/shaders
        DEPENDS ${SHADER_FILES}
)

add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:venturers>/assets
        DEPENDS ${ASSET_FILES}
)

add_dependencies(venturers copy_shaders copy_assets)

if (MSVC)
    target_compile_options(venturers PRIVATE /W4)
else()
    target_compile_options(venturers PRIVATE -Wall -Wextra -Wpedantic)
endif()