#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "ResourceManager.h"

Model::Model() {

}

Model::Model(const string& meshname) {
    SetShader("material");
    meshes = ResourceManager::GetMeshes(meshname);
    Init();
}

Model::Model(const string& meshname, vec3 position, vec3 rotation, vec3 size)
 : Position(position), Rotation(rotation), Size(size) {
    Rotation = glm::radians(Rotation);
    SetShader("material");
    meshes = ResourceManager::GetMeshes(meshname);
    Init();
}

void Model::Init() {
    
}

void Model::SetShader(string name) {
    shader = ResourceManager::GetShader(name);
}

void Model::Update(GLfloat dt) {
	
}

void Model::Draw(glm::mat4 projection, glm::mat4 view, LightingInfo& lighting) {
    shader.Use();

	currentModel = glm::mat4(1.0);
	currentModel = glm::scale(currentModel, Size);
	currentModel = glm::translate(currentModel, Position);
	glm::quat rot = glm::quat(Rotation);
	currentModel *= glm::toMat4(rot);
    

	//auto pvm = projection * view * currentModel;
    //shader.SetMatrix4("pvm", pvm);

    auto proj_view = projection * view;

    shader.SetMatrix4("pv", proj_view);
    shader.SetMatrix4("model", currentModel);
	
    for (Mesh mesh : meshes) {
	    mesh.Draw(shader, lighting);
    }
}