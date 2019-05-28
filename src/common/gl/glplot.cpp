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
    template <class RangeT>
    void draw(RangeT&& range) {
        std::vector<float> time(range.size());
        for (int i = 0; i < time.size(); ++i)
        {
            time[i] = (float(i) / (time.size() - 1)) * 2 - 1.f;
        }

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

        glLineWidth(2);
        glDrawArrays(GL_LINE_STRIP, 0, range.size());
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

void main()
{
    gl_Position = vec4(time, elem, 0, 1.0);
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

void enter(gl_plot& plotter)
{
    std::vector<float> v{0.3, 0.5, 0.1, 0.7, 0.2, 0.8, -0.2};
    plotter.draw(v);
}

void make_plotter(fastpl::gl::ctx& ctx)
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

    gl_plot plot{ctx, shader};

    enter(plot);
}
