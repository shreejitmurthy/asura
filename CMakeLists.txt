cmake_minimum_required(VERSION 3.29)
project(asura LANGUAGES C CXX)

option(ASURA_BUILD_SHARED "Build asura as a shared library" OFF)
option(ASURA_PROVIDE_SOKOL_IMPL "Compile a single SOKOL_IMPL TU inside asura" ON)
option(ASURA_ENABLE_WARNINGS "Enable reasonable warnings on engine sources" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(vendor/glm)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/json)

set(VENDOR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/)

set(ASURA_SOURCES
    "src/graphics.cc"
    "src/sprite.cc"
    "src/font.cc"
)

if(ASURA_PROVIDE_SOKOL_IMPL)
    if(APPLE)
        enable_language(OBJCXX)
        set(ASURA_SOKOL_TU "src/asura_sokol_impl.mm")
        add_library(asura_sokol_impl OBJECT ${ASURA_SOKOL_TU})
        target_compile_definitions(asura_sokol_impl PRIVATE SOKOL_IMPL SOKOL_METAL)
        target_include_directories(asura_sokol_impl PRIVATE ${VENDOR_INCLUDE_DIR})
    elseif(WIN32)
        set(ASURA_SOKOL_TU "src/asura_sokol_impl.c")
        add_library(asura_sokol_impl OBJECT ${ASURA_SOKOL_TU})
        target_compile_definitions(asura_sokol_impl PRIVATE SOKOL_IMPL SOKOL_D3D11)
        target_include_directories(asura_sokol_impl PRIVATE ${VENDOR_INCLUDE_DIR})
    else()
        set(ASURA_SOKOL_TU "src/asura_sokol_impl.c")
        add_library(asura_sokol_impl OBJECT ${ASURA_SOKOL_TU})
        target_compile_definitions(asura_sokol_impl PRIVATE SOKOL_IMPL SOKOL_GLCORE33)
        target_include_directories(asura_sokol_impl PRIVATE ${VENDOR_INCLUDE_DIR})
    endif()
endif()

if(ASURA_BUILD_SHARED)
    add_library(asura SHARED ${ASURA_SOURCES})
else()
    add_library(asura STATIC ${ASURA_SOURCES})
endif()

add_library(asura::engine ALIAS asura)

if(TARGET asura_sokol_impl)
    target_link_libraries(asura PRIVATE asura_sokol_impl)
endif()

target_include_directories(asura
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${VENDOR_INCLUDE_DIR}  # temporarily visible
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asura
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asura/gfx
        ${CMAKE_CURRENT_SOURCE_DIR}/include/asura/core
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
)

# These are also the libraries used across the core/
target_link_libraries(asura
    PUBLIC
        glm::glm
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

if(APPLE)
    enable_language(OBJCXX)
    target_link_libraries(asura PUBLIC
        "-framework Cocoa"
        "-framework QuartzCore"
        "-framework Metal"
        "-framework MetalKit"
    )
elseif(WIN32)
    target_link_libraries(asura PUBLIC d3d11 dxgi)
elseif(UNIX)
    target_link_libraries(asura PUBLIC OpenGL::GL)  # this needs to be tested on a linux computer
endif()

if(ASURA_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(asura PRIVATE /W4 /permissive-)
        if(TARGET asura_sokol_impl)
            target_compile_options(asura_sokol_impl PRIVATE /W4 /permissive-)
        endif()
    else()
        target_compile_options(asura PRIVATE -Wall -Wextra -Wpedantic)
        if(TARGET asura_sokol_impl)
            target_compile_options(asura_sokol_impl PRIVATE -Wall -Wextra -Wpedantic)
        endif()
    endif()
endif()

if(ASURA_BUILD_SHARED)
    include(GNUInstallDirs)
    set_target_properties(asura PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endif()

# TEMP:
target_compile_options(${PROJECT_NAME} PRIVATE -w)
