
cmake_minimum_required(VERSION 3.20)

project(UNO LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(UNO ${SOURCES})

target_include_directories(UNO PRIVATE include)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

if(EMSCRIPTEN)

    set(RAYLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raylib")

    add_subdirectory("${RAYLIB_DIR}" "${CMAKE_BINARY_DIR}/raylib")

    target_link_libraries(UNO PRIVATE raylib)

    set_target_properties(UNO PROPERTIES SUFFIX ".html")

    target_link_options(UNO PRIVATE
    "-sUSE_GLFW=3"
    "-sASYNCIFY"
    "-sALLOW_MEMORY_GROWTH=1"
    "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
    "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/shaders@/shaders"
    "--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html"
    )

else()

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

endif()
