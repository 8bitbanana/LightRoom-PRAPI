#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

map<string, Texture2D> ResourceManager::Textures;
map<string, Shader> ResourceManager::Shaders;
map<string, Mesh> ResourceManager::Meshes;


Shader ResourceManager::LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, string name)
{
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}

Shader ResourceManager::GetShader(string name)
{
	return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar* file, GLboolean alpha, string name)
{
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Texture2D ResourceManager::GetTexture(string name)
{
	return Textures[name];
}

Mesh ResourceManager::LoadMesh(string filename, string name) {
	Meshes[name] = loadMeshFromFile(filename);
	return Meshes[name];
}

Mesh ResourceManager::GetMesh(string name) {
	return Meshes[name];
}

glm::mat4 ResourceManager::AiToGlm(aiMatrix4x4 aiM) {
    float values[16] = {
        aiM.a1, aiM.a2, aiM.a3, aiM.a4,
        aiM.b1, aiM.b2, aiM.b3, aiM.b4,
        aiM.c1, aiM.c2, aiM.c3, aiM.c4,
        aiM.d1, aiM.d2, aiM.d3, aiM.d4
    };
    return glm::make_mat4(values);
}

glm::vec3 ResourceManager::AiToGlm(aiVector3D aiV) {
    return glm::vec3(aiV.x, aiV.y, aiV.z);
}

void ResourceManager::loadMeshesFromNode(const aiNode* node, aiMesh** meshes, glm::mat4 currentTransform, vector<AssimpMesh>* assimpmeshes) {
    // Add this node's transform to the stack
    currentTransform = currentTransform * AiToGlm(node->mTransformation);
    // Iterate through each mesh in this node
    for (unsigned int meshIndex=0; meshIndex<node->mNumMeshes; meshIndex++) {
        aiMesh* mesh = meshes[node->mMeshes[meshIndex]];
        // Construct the Mesh struct
        AssimpMesh meshStruct;
        meshStruct.Transform = currentTransform;
        // Add each vertex to the struct
        for (unsigned int vertIndex=0; vertIndex<mesh->mNumVertices; vertIndex++) {
            meshStruct.Vertices.push_back(AiToGlm(mesh->mVertices[vertIndex]));
        }
        // Add each index inside each face to the struct
        for (unsigned int faceIndex=0; faceIndex<mesh->mNumFaces; faceIndex++) {
            aiFace face = mesh->mFaces[faceIndex];
            // aiProcess_Triangulate makes sure that all faces are triangles
            assert(face.mNumIndices == 3);
            for (unsigned int i=0; i<face.mNumIndices; i++) {
                meshStruct.Indices.push_back(face.mIndices[i]);
            }
        }
        assimpmeshes->push_back(meshStruct);
    }

    // Recursively run this function for all of this node's children
    for (unsigned int childIndex=0; childIndex<node->mNumChildren; childIndex++) {
        aiNode* child = node->mChildren[childIndex];
        loadMeshesFromNode(child, meshes, currentTransform, assimpmeshes);
    }
}

Mesh ResourceManager::loadMeshFromFile(string filename) {
    Mesh outmesh;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename,
        // aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType
    );

    if (!scene) {
        fprintf(stderr, "ASSIMP ERROR - %s\n", importer.GetErrorString());
        return outmesh;
    }

    vector<AssimpMesh> assimpMeshes;
    loadMeshesFromNode(scene->mRootNode, scene->mMeshes, glm::mat4(1.0), &assimpMeshes);
    
    // Extract the verts from the meshes into one vector
    unsigned int indexOffset = 0;
    vector<glm::vec3> verts;
    vector<GLuint> inds;

    for (auto mesh : assimpMeshes) {
        for (auto vert : mesh.Vertices) {
            glm::vec4 vert4 = glm::vec4(vert, 1);
            verts.push_back((glm::vec3)(vert4 * mesh.Transform));
        }   
        for (auto ind : mesh.Indices) {
            inds.push_back(ind + indexOffset);
        }
        indexOffset += mesh.Vertices.size();
    }
    outmesh.Import(verts, inds);
    return outmesh;
}

void ResourceManager::Clear()
{
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
	Shaders.clear();
	Textures.clear();
	Meshes.clear();
}

Shader ResourceManager::loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile)
{
	string vertexCode;
	string fragmentCode;
	string geometryCode;
	try
	{
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		vertexShaderFile.close();
		fragmentShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		if (gShaderFile != nullptr) {
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	const GLchar* gShaderCode = geometryCode.c_str();
	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar* file, GLboolean alpha)
{
	Texture2D texture;
	if (alpha) {
		texture.Internal_Format = GL_RGBA;
		texture.Image_Format = GL_RGBA;
	}
	int width, height;
	//unsigned char* image = SOIL_load_image(file, &width, &height, 0, texture.Image_Format == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
	

	unsigned char* image = stbi_load(file, &width, &height, (int *)(texture.Image_Format == GL_RGBA ? STBI_rgb_alpha : STBI_rgb), 0);

	texture.Generate(width, height, image);
	//SOIL_free_image_data(image);
	stbi_image_free(image);
	return texture;
}
