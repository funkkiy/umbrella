cmake_minimum_required(VERSION 3.15)

project(Umbrella LANGUAGES CXX)

string(CONCAT WALL_OTHERS "$<"
    "$<AND:"
        "$<COMPILE_LANGUAGE:CXX>,"
        "$<NOT:"
            "$<CXX_COMPILER_ID:MSVC>"
        ">"
    ">:"
    "-Wall -Wextra"
">")

string(CONCAT WALL_MSVC "$<"
    "$<AND:"
        "$<COMPILE_LANGUAGE:CXX>,"
        "$<CXX_COMPILER_ID:MSVC>"
    ">:"
    "/W4"
">")

# enable folder hierarchy: filters on msvc
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# output to current binary directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw)

# umbrella target
list(APPEND UMBRELLA_SOURCES
    # umbrella
    src/main.cpp
    src/umbrella/UmbrellaApplication.cpp
    src/umbrella/UmbrellaApplication.h

    # glad
    vendor/glad/src/gl.c
)

list(APPEND UMBRELLA_INCLUDES
    # umbrella
    src/umbrella

    # glad
    vendor/glad/include
)

add_executable(Umbrella)
target_sources(Umbrella PRIVATE
    ${UMBRELLA_SOURCES}
)
target_include_directories(Umbrella PRIVATE
    ${UMBRELLA_INCLUDES}
)
target_link_libraries(Umbrella glfw)
target_compile_features(Umbrella PRIVATE cxx_std_20)
target_compile_options(Umbrella PRIVATE
    ${WALL_OTHERS} ${WALL_MSVC}
)

# msvc-specific Umbrella settings
if(MSVC)
    set_target_properties(Umbrella PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
    )
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY VS_STARTUP_PROJECT Umbrella
    )
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${UMBRELLA_SOURCES})
endif()
