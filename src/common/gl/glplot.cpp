//
// Created by fatih on 5/27/19.
//

#include <rtk/gl/program.hpp>

#include <fastplot/gl/ctx.hpp>
#include <vector>

#include <glm/glm.hpp>
#include <numeric>
#include <rtk/gl/shader.hpp>
#include <gsl/span>

#include <glad.h>

class gl_plot
{
public:
    void draw_line(
        gsl::span<const float> time,
        gsl::span<const float> range,
        glm::vec3 color,
        int line_width,
        float val_min,
        float val_max);

    void draw_points(
            gsl::span<const float> time,
            gsl::span<const float> range,
            glm::vec3 color,
            float size,
            float val_min,
            float val_max);

    void draw_line(gsl::span<const float> range);
    void draw_points(gsl::span<const float> range);

    gl_plot(fastpl::gl::ctx& ctx);

private:
    void draw_line_x(float x, float min, float max)
    {
        const std::array<float, 2> time { x, x };
        constexpr std::array<float, 2> range { -1, 1 };

        draw_line(time, range, glm::vec3{0.7, 0.7, 0.7}, 1, min, max);
    }

    void draw_line_y(float y, float min, float max)
    {
        constexpr std::array<float, 2> time { -1, 1 };
        const std::array<float, 2> range { y, y };

        draw_line(time, range, glm::vec3{0.7, 0.7, 0.7}, 1, min, max);
    }

    fastpl::gl::ctx* m_ctx;
    rtk::gl::program m_line_prog;
    rtk::gl::program m_point_prog;
    GLuint quad_vbo;
};

void gl_plot::draw_line(gsl::span<const float> time, gsl::span<const float> range, glm::vec3 color, int line_width,
                        float val_min, float val_max) {
    GLuint bufs[2];

    glGenBuffers(2, bufs);

    glBindBuffer(GL_ARRAY_BUFFER, bufs[0]);
    glBufferData(GL_ARRAY_BUFFER, time.size_bytes(), time.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufs[1]);
    glBufferData(GL_ARRAY_BUFFER, range.size_bytes(), range.data(), GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, bufs[0]);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, bufs[1]);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);

    m_line_prog.use();
    m_line_prog.set_variable("Color", color);
    m_line_prog.set_variable("Min", val_min);
    m_line_prog.set_variable("Max", val_max);

    for (int i = -line_width / 2; i <= line_width / 2; ++i)
    {
        m_line_prog.set_variable("Shift", float(i) / 1500);
        glDrawArrays(GL_LINE_STRIP, 0, range.size());
    }

    glDeleteBuffers(2, bufs);
    glDeleteVertexArrays(1, &vao);
}

void gl_plot::draw_points(gsl::span<const float> time, gsl::span<const float> range, glm::vec3 color, float size,
                          float val_min, float val_max) {
    GLuint bufs[2];

    glGenBuffers(2, bufs);

    glBindBuffer(GL_ARRAY_BUFFER, bufs[0]);
    glBufferData(GL_ARRAY_BUFFER, time.size_bytes(), time.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufs[1]);
    glBufferData(GL_ARRAY_BUFFER, range.size_bytes(), range.data(), GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, bufs[0]);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, bufs[1]);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);

    m_point_prog.use();
    m_point_prog.set_variable("Color", color);
    m_point_prog.set_variable("Min", val_min);
    m_point_prog.set_variable("Max", val_max);
    m_point_prog.set_variable("Scale", size);

    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, time.size());

    glDeleteBuffers(2, bufs);
    glDeleteVertexArrays(1, &vao);
}

void gl_plot::draw_line(gsl::span<const float> range) {
    std::vector<float> time(range.size());

    for (int i = 0; i < time.size(); ++i)
    {
        time[i] = (float(i) / (time.size() - 1)) * 2 - 1.f;
    }

    auto [val_min, val_max] = std::minmax_element(range.begin(), range.end());
    auto mean = std::accumulate(range.begin(), range.end(), (double)0) / range.size();

    draw_line_x(-1, -1, 1);
    draw_line_x(1, -1, 1);
    draw_line_y(-1, -1, 1);
    draw_line_y(1, -1, 1);

    draw_line_y(mean, *val_min, *val_max);
    draw_line(time, range, glm::vec3{1, 1, 0}, 5, *val_min, *val_max);
}

void gl_plot::draw_points(gsl::span<const float> range) {
    std::vector<float> time(range.size());

    for (int i = 0; i < time.size(); ++i)
    {
        time[i] = (float(i) / (time.size() - 1)) * 2 - 1.f;
    }

    auto [val_min, val_max] = std::minmax_element(range.begin(), range.end());
    auto mean = std::accumulate(range.begin(), range.end(), (double)0) / range.size();

    draw_line_x(-1, -1, 1);
    draw_line_x(1, -1, 1);
    draw_line_y(-1, -1, 1);
    draw_line_y(1, -1, 1);

    draw_line_y(mean, *val_min, *val_max);
    draw_points(time, range, glm::vec3{1, 1, 0}, 0.1, *val_min, *val_max);
}

GLuint load_quad(gsl::span<const float> quad_vertices)
{
    GLuint VBO;

    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, quad_vertices.size_bytes(), quad_vertices.data(), GL_STATIC_DRAW);

    return VBO;
}

constexpr auto vert_line = R"__(#version 300 es
precision mediump float;
in float time;
in float elem;

uniform float Min;
uniform float Max;

uniform float Shift;

void main()
{
    float tim = ((time + Shift + 1.f) / 2.f) * 1.8f - 0.9f;
    float val = ((elem - Min) / (Max - Min)) * 1.8f - 0.9f;
    gl_Position = vec4(tim, val, 0, 1.0);
}
)__";

constexpr auto frag_line = R"__(#version 300 es
precision mediump float;
out vec4 FragColor;
uniform vec3 Color;

void main()
{
    FragColor = vec4(Color, 1.0f);
}
)__";

constexpr auto vert_point = R"__(#version 300 es
precision mediump float;
layout (location = 0) in float time;
layout (location = 1) in float elem;
layout (location = 2) in vec2 pos; // constant

uniform float Min;
uniform float Max;

uniform float Scale;

void main()
{
    float tim = ((time + 1.f) / 2.f) * 1.8f - 0.9f;
    float val = ((elem - Min) / (Max - Min)) * 1.8f - 0.9f;

    vec2 position = pos * Scale + vec2(tim, val);

    gl_Position = vec4(position.x, position.y, 0, 1.0);
}
)__";

constexpr auto frag_point = R"__(#version 300 es
precision mediump float;
out vec4 FragColor;
uniform vec3 Color;

void main()
{
    FragColor = vec4(Color, 1.0f);
}
)__";

gl_plot::gl_plot(fastpl::gl::ctx &ctx) : m_ctx{&ctx}
{
    rtk::gl::vertex_shader line_vs{vert_line};
    rtk::gl::fragment_shader line_fs{frag_line};
    m_line_prog.attach(line_vs);
    m_line_prog.attach(line_fs);
    m_line_prog.link();

    rtk::gl::vertex_shader point_vs{vert_point};
    rtk::gl::fragment_shader point_fs{frag_point};
    m_point_prog.attach(point_vs);
    m_point_prog.attach(point_fs);
    m_point_prog.link();

    float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,
        0.05f, -0.05f,
        -0.05f, -0.05f,

        -0.05f,  0.05f,
        0.05f, -0.05f,
        0.05f,  0.05f,
    };

    quad_vbo = load_quad(quadVertices);
}

std::vector<float> v{0.3, 0.5};

void enter(gl_plot& plotter)
{
    if (v.size() >= 120)
    {
        v.erase(v.begin());
    }
    v.push_back((rand() % 255) - 128);
    plotter.draw_line(v);
    plotter.draw_points(v);
}

std::unique_ptr<gl_plot> plotter;

void make_plotter(fastpl::gl::ctx& ctx)
{
    if (!plotter)
    {
        ctx.activate();
        plotter = std::make_unique<gl_plot>(ctx);
    }

    enter(*plotter);
}