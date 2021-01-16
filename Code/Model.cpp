#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ResourceManager.h"

#define USE_EXAMPLE_LAMPS

Model::Model() {

}

Model::Model(const string& meshname) {
    SetShader("material");
    auto data = ResourceManager::GetModelData(meshname);
    meshes = data.meshes;
    lamps = data.lamps;
    Init();
}

Model::Model(const string& meshname, vec3 position, vec3 rotation, vec3 size)
 : Position(position), Rotation(rotation), Size(size) {
    Rotation = glm::radians(Rotation);
    SetShader("material");
    auto data = ResourceManager::GetModelData(meshname);
    meshes = data.meshes;
    lamps = data.lamps;
    Init();
}

void Model::Init() {
    #ifdef USE_EXAMPLE_LAMPS
    lamps.clear();
    lamps.push_back(ModelLamp {
        glm::vec3(1.2f, 1.0f, 2.0f),
        glm::vec4(0,1,0,1)
    });
    lamps.push_back(ModelLamp {
        glm::vec3(1.2f, 1.0f, 2.0f),
        glm::vec4(1,0,0,1)
    });
    lamps.push_back(ModelLamp {
        glm::vec3(1.2f, 1.0f, 2.0f),
        glm::vec4(0.2,0.2,1,1)
    });
    #endif
}

void Model::SetShader(string name) {
    shader = ResourceManager::GetShader(name);
}

void Model::Update(GLfloat dt) {
    #ifdef USE_EXAMPLE_LAMPS
    const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
	lamps[0].Position = glm::vec3(glm::mat4_cast(glm::angleAxis(5*dt, UP)) * glm::vec4(lamps[0].Position, 1));
	lamps[1].Position = glm::vec3(glm::mat4_cast(glm::angleAxis(-3*dt, UP)) * glm::vec4(lamps[1].Position, 1));
	lamps[2].Position = glm::vec3(glm::mat4_cast(glm::angleAxis(8*dt, UP)) * glm::vec4(lamps[2].Position, 1));
    #endif
}

void Model::Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 viewPos) {
    shader.Use();

	currentModel = glm::mat4(1.0);
	currentModel = glm::scale(currentModel, Size);
	currentModel = glm::translate(currentModel, Position);
	glm::quat rot = glm::quat(Rotation);
	currentModel *= glm::toMat4(rot);
    
    LightingInfo lighting;
    for (int i=0; i<MAX_LIGHTS; i++) {
        if (i < lamps.size()) {
            lighting.LightActive[i] = true;
            lighting.LightColor[i] = glm::vec4(lamps[i].Color, 1);
            lighting.LightPos[i] = lamps[i].Position;
            lighting.SpecularStrength[i] = 0.5f;
        } else {
            lighting.LightActive[i] = false;
        }
    }
    
    lighting.AmbientColor = glm::vec4(1);
    lighting.AmbientStrength = 0.2f;
    lighting.ViewPos = viewPos;
	//auto pvm = projection * view * currentModel;
    //shader.SetMatrix4("pvm", pvm);

    auto proj_view = projection * view;

    shader.SetMatrix4("pv", &proj_view);
    shader.SetMatrix4("model", &currentModel);
	
    for (Mesh mesh : meshes) {
	    mesh.Draw(shader, lighting);
    }
}