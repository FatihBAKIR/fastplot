//
// Created by fatih on 5/27/19.
//

#include <rtk/gl/program.hpp>

#include <fastplot/gl/ctx.hpp>
#include <vector>

#include <glm/glm.hpp>
#include <numeric>
#include <rtk/gl/shader.hpp>

class gl_plot
{
public:
    void draw(
            const std::vector<float>& time,
            const std::vector<float>& range,
            glm::vec3 color,
            int line_width,
            float val_min,
            float val_max)
    {
        GLuint bufs[2];

        glGenBuffers(2, bufs);

        glBindBuffer(GL_ARRAY_BUFFER, bufs[0]);
        glBufferData(GL_ARRAY_BUFFER, time.size() * sizeof(float), time.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, bufs[1]);
        glBufferData(GL_ARRAY_BUFFER, range.size() * sizeof(float), range.data(), GL_STATIC_DRAW);

        GLuint vao;
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, bufs[0]);
        glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, bufs[1]);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);

        m_prog->use();
        m_prog->set_variable("Color", color);
        m_prog->set_variable("Min", val_min);
        m_prog->set_variable("Max", val_max);

        glLineWidth(line_width);
        glDrawArrays(GL_LINE_STRIP, 0, range.size());

        glDeleteBuffers(2, bufs);
        glDeleteVertexArrays(1, &vao);
    }

    void draw(const std::vector<float>& range) {
        std::vector<float> time(range.size());

        for (int i = 0; i < time.size(); ++i)
        {
            time[i] = (float(i) / (time.size() - 1)) * 2 - 1.f;
        }

        auto [val_min, val_max] = std::minmax_element(range.begin(), range.end());

        draw(time, range, glm::vec3{1, 1, 0}, 10, *val_min, *val_max);
    }

    gl_plot(fastpl::gl::ctx& ctx, std::shared_ptr<rtk::gl::program> prog)
        : m_ctx{&ctx}, m_prog{std::move(prog)} {}

private:
    fastpl::gl::ctx* m_ctx;
    std::shared_ptr<rtk::gl::program> m_prog;
};

constexpr auto vert = R"__(#version 300 es
precision mediump float;
in float time;
in float elem;

uniform float Min;
uniform float Max;

void main()
{
    float val = ((elem - Min) / (Max - Min)) * 1.8f - 0.9f;
    gl_Position = vec4(time, val, 0, 1.0);
}
)__";

constexpr auto frag = R"__(#version 300 es
precision mediump float;
out vec4 FragColor;
uniform vec3 Color;

void main()
{
    FragColor = vec4(Color, 1.0f);
}
)__";

std::vector<float> v{0.3, 0.5};

void enter(gl_plot& plotter)
{
    if (v.size() >= 1'000)
    {
        v.erase(v.begin());
    }
    v.push_back(rand());
    plotter.draw(v);
}

std::unique_ptr<gl_plot> plotter;

void make_plotter(fastpl::gl::ctx& ctx)
{
    if (!plotter)
    {
        ctx.activate();

        auto shader = std::make_shared<rtk::gl::program>();
        {
            rtk::gl::vertex_shader mesh_vs{vert};
            rtk::gl::fragment_shader mesh_fs{frag};
            shader->attach(mesh_vs);
            shader->attach(mesh_fs);
            shader->link();
        }

        plotter = std::make_unique<gl_plot>(ctx, shader);
    }

    enter(*plotter);
}
