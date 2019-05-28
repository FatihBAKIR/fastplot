//
// Created by fatih on 23.03.2017.
//

#pragma once

#include <string>
#include <glad/glad.h>
#include <stdexcept>

namespace rtk {
namespace gl {
namespace shaders {
struct fragment
{
    static constexpr auto shader_type = GL_FRAGMENT_SHADER;
};

struct vertex
{
    static constexpr auto shader_type = GL_VERTEX_SHADER;
};
}

template<class ShaderT>
class shader
{
    GLuint id = 0;

public:
    shader() = default;
    shader(const shader&) = delete;

    shader(shader&& rhs)
    {
        id = rhs.id;
        rhs.id = 0;
    }

    shader(const char* src)
    {
        id = 0;
        load(src);
    }

    void load(const char* src)
    {
        if (id) {
            clear();
        }

        id = glCreateShader(ShaderT::shader_type);
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        GLint status;
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);

        if (status!=GL_TRUE) {
            char buffer[512];
            glGetShaderInfoLog(id, 512, NULL, buffer);

            throw std::runtime_error(std::string("shader compile error: ")+buffer);
        }

        //Ensures(glIsShader(id));
    }

    void clear()
    {
        glDeleteShader(id);
    }

    GLuint get_id() const
    {
        return id;
    }

    ~shader()
    {
        clear();
    }
};
}
}