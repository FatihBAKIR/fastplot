//
// Created by fatih on 5/27/19.
//

#pragma once

#include <memory>

namespace fastpl
{
namespace gl
{
struct arch_ctx;

class ctx {
public:
    void activate();

    void begin_draw();
    void end_draw();

    ~ctx();

    ctx(ctx&&) = default;

private:

    explicit ctx(std::unique_ptr<arch_ctx> impl) : m_impl{std::move(impl)} {}

    std::unique_ptr<arch_ctx> m_impl;
    friend gl::ctx make_ctx();
};
    gl::ctx make_ctx();
}
}