#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

using std::vector;

class Mesh {
public:
    Mesh();
    void Import(vector<glm::vec3> vertices, vector<GLuint> indices);
    void Draw();

    vector<glm::vec3> Vertices;
    vector<GLuint> Indices;
private:
    unsigned int VBO, VAO, EBO;
};