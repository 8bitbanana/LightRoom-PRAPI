#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

#include "Shader.h"
#include "Texture.h"

using std::vector;

struct MeshVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
public:
    Mesh();
    void Import(vector<MeshVertex> vertices, vector<GLuint> indices, vector<Texture2D> textures);
    void Draw(Shader& shader);

    vector<MeshVertex> Vertices;
    vector<GLuint> Indices;
    vector<Texture2D> Textures;
    glm::vec4 DiffuseColor;
private:
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
};