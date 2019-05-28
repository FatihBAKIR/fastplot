//
// Created by fatih on 5/27/19.
//

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/em_asm.h>
#include <iostream>
#include <glad/glad.h>
#include <fastplot/gl/ctx.hpp>
#include <rtk/gl/program.hpp>
#include <rtk/gl/shader.hpp>

using namespace fastpl;

struct gl::arch_ctx {
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_handle;
};

gl::ctx::~ctx() = default;

void gl::ctx::activate() {
    emscripten_webgl_make_context_current(m_impl->m_handle);
}

void fastpl::gl::ctx::begin_draw() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void fastpl::gl::ctx::end_draw() {
    emscripten_webgl_commit_frame();
}

extern "C" { extern void* emscripten_GetProcAddress(const char *x); }

gl::ctx fastpl::gl::make_ctx()
{
    EmscriptenWebGLContextAttributes attr{};
    emscripten_webgl_init_context_attributes(&attr);

    attr.antialias = true;
    attr.depth = true;
    attr.stencil = false;
    attr.alpha = true;
    attr.majorVersion = 2;
    attr.minorVersion = 0;
    attr.explicitSwapControl = true;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);

    if (ctx <= 0)
    {
        std::cerr << ctx << " error\n";
        throw std::runtime_error("Error");
    }

    auto ac = std::make_unique<gl::arch_ctx>();
    ac->m_handle = ctx;

    auto res = gl::ctx{ std::move(ac) };

    res.activate();
    if (!gladLoadGLLoader(emscripten_GetProcAddress))
    {
        std::cerr << "could not initialize glad\n";

        throw std::runtime_error("could not initialize glad");
    }

    return std::move(res);
}

constexpr auto vert = R"__(#version 300 es
precision mediump float;
in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)__";

constexpr auto frag = R"__(#version 300 es
precision mediump float;
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)__";

gl::ctx* c;
unsigned int VBO;
unsigned int VAO;
std::shared_ptr<rtk::gl::program> shader;

int main()
{
    c = new gl::ctx(gl::make_ctx());

    c->activate();

    shader = std::make_shared<rtk::gl::program>();

    {
        rtk::gl::vertex_shader mesh_vs { vert };
        rtk::gl::fragment_shader mesh_fs { frag };
        shader->attach(mesh_vs);
        shader->attach(mesh_fs);
        shader->link();
    }

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
// 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 3. then set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    emscripten_set_main_loop([]{
        c->activate();
        c->begin_draw();

        shader->use();

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        c->end_draw();
    }, 0, false);
}