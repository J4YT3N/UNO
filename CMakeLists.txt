cmake_minimum_required(VERSION 3.20)

project(UNO LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

add_executable(UNO ${SOURCES})

target_include_directories(UNO PRIVATE include)

add_library(raylib STATIC IMPORTED)

set_target_properties(raylib PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libraylib.a"
)

target_link_libraries(UNO PRIVATE
    raylib
    "-framework Cocoa"
    "-framework IOKit"
    "-framework OpenGL"
    "-framework CoreVideo"
    "-framework CoreAudio"
    "-framework AudioToolbox"
    "-framework QuartzCore"
)