#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RenderTools.hpp"
#include "VertexDefinitions.hpp"

namespace Render {
namespace UI {
class Source {
  public:
    Source();
    ;
    void BufferData(const ::UI::Vertex data[], int elements);
    ~Source();
    void Draw();

  private:
    unsigned int vbo;
    unsigned int vao;
    int elements = -1;
    const int stride = 5;
};

class Renderer {
  public:
    Renderer();
    void Draw(Source &source, const glm::mat4 &projection) const;

  private:
    const int shaderProgramId;
    const int projectionlocation;
};
} // namespace UI
} // namespace Render
