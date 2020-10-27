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


struct AssimpMesh {
    vector<glm::vec3> Vertices;
    vector<GLuint> Indices;
    glm::mat4x4 Transform;
};

class ResourceManager
{
public:
	static map<string, Shader> Shaders;
	static map<string, Texture2D> Textures;
	static map<string, Mesh> Meshes;

	static Shader LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, string name);
	static Shader GetShader(string name);
	static Texture2D LoadTexture(const GLchar* file, GLboolean alpha, string name);
	static Texture2D GetTexture(string name);
	static Mesh LoadMesh(string filename, string name);
	static Mesh GetMesh(string name);
	
	static void Clear();
private:
	ResourceManager() {}

	static Shader loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile = nullptr);
	static Texture2D loadTextureFromFile(const GLchar* file, GLboolean alpha);
	static Mesh loadMeshFromFile(std::string filename);

	static void loadMeshesFromNode(const aiNode* node, aiMesh** meshes, glm::mat4 currentTransform, vector<AssimpMesh>* assimpmeshes);

    static glm::mat4 AiToGlm(aiMatrix4x4 aiM);
    static glm::vec3 AiToGlm(aiVector3D aiV);
};

