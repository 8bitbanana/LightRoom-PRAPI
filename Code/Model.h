#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "ResourceManager.h"
#include "LightingInfo.h"

using std::string;
using std::vector;
using glm::vec3;

class Model {
public:
    Model();
    Model(const string& mesh);
    Model(const string& mesh, vec3 position);
    Model(const string& mesh, vec3 position, vec3 rotation, vec3 size);

    virtual void Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 viewPos);
	virtual void Update(GLfloat dt);

    void SetShader(string name);

    glm::vec3 Position = glm::vec3(0);
    glm::vec3 Rotation = glm::vec3(0);
    glm::vec3 Size = glm::vec3(1);
protected:
    glm::highp_mat4 currentModel;
    vector<Mesh> meshes;
    vector<ModelLamp> lamps;
private:
    unsigned int VBO, VAO, EBO;
    Shader shader;

    void Init();
};