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

#include <emscripten/bind.h>

using namespace emscripten;

using namespace fastpl;

gl::ctx::ctx(std::unique_ptr<fastpl::gl::arch_ctx> impl) : m_impl{std::move(impl)} {}
gl::ctx::ctx(fastpl::gl::ctx &&) noexcept = default;

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

gl::ctx make_ctx(const std::string& canvas)
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
    attr.antialias = false;

    std::cerr << "canvas: " << canvas << '\n';
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context(canvas.c_str(), &attr);

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

gl::ctx fastpl::gl::make_ctx()
{
    return ::make_ctx("#canvas");
}

using ctx_ptr = std::shared_ptr<fastpl::gl::ctx>;
ctx_ptr make_ctx_wrapper(const std::string& canvas)
{
    return std::make_shared<fastpl::gl::ctx>(make_ctx(canvas));
}

void make_plotter(fastpl::gl::ctx& ctx);

void draw(ctx_ptr ptr)
{
    ptr->activate();
    ptr->begin_draw();

    make_plotter(*ptr);

    ptr->end_draw();
}

EMSCRIPTEN_BINDINGS(fastplot)
{
    class_<fastpl::gl::ctx>("context")
        .smart_ptr<ctx_ptr>("context");
    function("make_ctx", &make_ctx_wrapper);
    function("draw", &draw);
}