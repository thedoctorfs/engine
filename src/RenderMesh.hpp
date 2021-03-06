#pragma once

#include "VertexDefinitions.hpp"
#include <glm/glm.hpp>

namespace Render {
namespace Mesh {
class Source {
  public:
    Source();
    void BufferData(const ::Mesh::Vertex data[], int elements);
    ~Source();
    void Draw();

  private:
    unsigned int vbo;
    unsigned int vao;
    int elements = -1;
};

class Renderer {
  public:
    Renderer();
    ~Renderer();
    void Draw(Source &source, const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection, float selectioncolor) const;

  private:
    const int shaderProgramId;
    const int modellocation;
    const int viewlocation;
    const int projectionlocation;
    const int selectioncolorlocation;
};
} // namespace Mesh
} // namespace Render
