#pragma once

#include <map>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

using std::string;
using std::map;
using std::vector;

struct ModelLamp {
    glm::vec3 Position;
    glm::vec3 Color;
};

struct ModelData {
    vector<Mesh> meshes;
    vector<ModelLamp> lamps;
};

struct AssimpMesh {
    vector<MeshVertex> Vertices;
    vector<GLuint> Indices;
	vector<Texture2D> Textures;
    glm::mat4x4 Transform;
	glm::vec4 DiffuseColor;
	glm::vec4 SpecularColor;
	glm::vec4 AmbientColor;
	glm::vec4 EmissiveColor;
	glm::vec4 TransparentColor;
};

class ResourceManager
{
public:
	static map<string, Shader> Shaders;
	static map<string, Texture2D> Textures;
	static map<string, ModelData> Models;

	static Shader LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, string name);
	static Shader GetShader(string name);
	static Texture2D LoadTexture(const GLchar* file, string name, GLboolean alpha = GL_FALSE, Texture2D::TextureType textype = Texture2D::TextureType::DIFFUSE);
	static Texture2D GetTexture(string name);
	static ModelData LoadModelData(string filename, string name);
	static ModelData GetModelData(string name);
	
	static void Clear();
private:
	ResourceManager() {}

	static Shader loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile = nullptr);
	static Texture2D loadTextureFromFile(const GLchar* file, GLboolean alpha, Texture2D::TextureType textype);
	static ModelData loadModelDataFromFile(std::string filename);

	static void loadObjectsFromNode(const aiNode* node, const aiScene* scene, glm::mat4 currentTransform, vector<AssimpMesh>* assimpmeshes);
	static vector<Texture2D> loadMaterialTextures(aiMaterial *mat, aiTextureType type);

	static Texture2D::TextureType AiToTex2D(aiTextureType aiT);

    static glm::mat4 AiToGlm(aiMatrix4x4 aiM);
	static glm::vec2 AiToGlm(aiVector2D aiV);
    static glm::vec3 AiToGlm(aiVector3D aiV);
	static glm::vec4 AiToGlm(aiColor3D aiC);
	static glm::vec4 AiToGlm(aiColor4D aiC);
};

