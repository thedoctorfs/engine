#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RenderTools.h"

namespace Text
{
struct Vertex
{
    Vertex(glm::vec2 pos, glm::vec3 col, glm::vec2 texcoord)
    : pos(pos)
    , col(col)
    , texcoord(texcoord)
    {
    }
    glm::vec2 pos;
    glm::vec3 col;
    glm::vec2 texcoord;
};

class Source
{
public:
	Source()
	{
        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
	}
    template <typename T>
    void BufferData(const T& data, int size)
    {
        this->elements = size / stride;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, &data[0], GL_STATIC_DRAW);
    }
    ~Source()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
    void Draw()
    {
        assert(elements != -1);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, elements);
        glBindVertexArray(0);
    }
    unsigned int vbo;
    unsigned int vao;
    int elements = -1;
    const int stride = 7;
};

class Renderer
{
public:
	Renderer()
    : shaderProgramId(CompileLinkVertexFragmentShader(vss, fss))
	, projectionlocation(glGetUniformLocation(shaderProgramId, "projection"))
	{
	}
	void Draw(Source& source, const glm::mat4& projection) const
	{
		glUseProgram(shaderProgramId);
        glUniformMatrix4fv(projectionlocation, 1, false, glm::value_ptr(projection));
        source.Draw();
	}

private:
    const int shaderProgramId;
    const int projectionlocation;

static constexpr char* vss =
"#version 330 core\n"
"layout (location = 0) in vec2 position;\n"
"layout (location = 1) in vec3 color;\n"
"layout (location = 2) in vec2 coord;\n"
"out vec3 ourColor;\n"
"out vec2 ourCoord;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"  gl_Position = projection * vec4(position, 0.0f, 1.0f);\n"
"  ourColor = color;\n"
"  ourCoord = coord;\n"
"}\n";

static constexpr char* fss =
"#version 330 core\n"
"in vec3 ourColor;\n"
"in vec2 ourCoord;\n"
"uniform sampler2D tex;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"  color = vec4(texture(tex, ourCoord).r, 0.0f, 0.0f, 1.0f);\n"
//"  color = vec4(ourColor, 1.0f);\n"
"}\n";
};
}


#endif
