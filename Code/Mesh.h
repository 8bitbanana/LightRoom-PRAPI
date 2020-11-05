#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

#include "Shader.h"

using std::vector;

class Mesh {
public:
    Mesh();
    void Import(vector<glm::vec3> vertices, vector<GLuint> indices);
    void Draw(Shader& shader);

    vector<glm::vec3> Vertices;
    vector<GLuint> Indices;
    glm::vec4 DiffuseColor;
private:
    unsigned int VBO, VAO, EBO;
};