cmake_minimum_required(VERSION 3.8)
project(fastplot)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    set(EMSCRIPTEN TURE)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
endif()

find_package(GLM REQUIRED)
find_package(glad REQUIRED)

add_library(fastplot_gl src/common/gl/glplot.cpp include/fastplot/gl/ctx.hpp include/rtk/gl/shader.hpp include/rtk/gl/program.hpp include/rtk/rtk_fwd.hpp src/rtk/gl/program.cpp)
target_include_directories(fastplot_gl PUBLIC ${GLM_INCLUDE_DIRS})
target_include_directories(fastplot_gl PUBLIC "include")
target_compile_features(fastplot_gl PUBLIC cxx_std_17)
target_compile_options(fastplot_gl PUBLIC "-std=c++17")
target_link_libraries(fastplot_gl PUBLIC glad::glad)
if (NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED COMPONENTS EGL)
    target_link_libraries(fastplot_gl PUBLIC OpenGL::EGL)
elseif()
    target_compile_definitions(fastplot_gl PUBLIC EMSCRIPTEN=1)
endif()

add_library(fastplot_common include/fastplot/fastplot.hpp src/common/plot.cpp)
target_compile_features(fastplot_common PUBLIC cxx_std_17)

if (NOT EMSCRIPTEN)
    add_subdirectory(src/glfw)
endif()

if (EMSCRIPTEN)
    add_subdirectory(src/emscripten)
endif()