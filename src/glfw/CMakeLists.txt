find_package(glfw3 REQUIRED)
add_executable(glfwplot glfw_ctx.cpp)
target_compile_definitions(glfwplot PUBLIC GLFW_INCLUDE_NONE=1)
target_link_libraries(glfwplot PUBLIC fastplot_gl glfw)
