#include <iostream>
#include <fastplot/gl/ctx.hpp>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

using namespace fastpl;

struct gl::arch_ctx {
    GLFWwindow* m_wnd;
};

gl::ctx::~ctx() = default;

void gl::ctx::activate() {
    glfwMakeContextCurrent(m_impl->m_wnd);
}

void fastpl::gl::ctx::begin_draw() {
    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void fastpl::gl::ctx::end_draw() {
    glfwSwapBuffers(m_impl->m_wnd);
}

gl::ctx fastpl::gl::make_ctx()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    auto m_wnd = glfwCreateWindow(640, 480, "FastPlot", nullptr, nullptr);
    glfwSetErrorCallback([](int error, const char *msg){
        throw std::runtime_error(msg);
    });

    if (!m_wnd)
    {
        throw std::runtime_error("Error:");
    }

    auto ac = std::make_unique<gl::arch_ctx>();
    ac->m_wnd = m_wnd;

    auto res = gl::ctx{ std::move(ac) };

    res.activate();
    if (!gladLoadGLLoader([](const char* nm) -> void* {
        return reinterpret_cast<void*>(glfwGetProcAddress(nm));
    }))
    {
        throw std::runtime_error("could not initialize glad");
    }

    return std::move(res);
}
int main() {
    auto context = gl::make_ctx();
    while (true)
    {
        context.activate();
        context.begin_draw();
        context.end_draw();
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}