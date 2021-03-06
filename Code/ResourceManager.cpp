#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using std::string;

map<string, Texture2D> ResourceManager::Textures;
map<string, Shader> ResourceManager::Shaders;
map<string, ModelData> ResourceManager::Models;


Shader ResourceManager::LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, string name)
{
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}

Shader ResourceManager::GetShader(string name)
{
	return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar* file, string name, GLboolean alpha, Texture2D::TextureType textype)
{
	Textures[name] = loadTextureFromFile(file, alpha, textype);
	return Textures[name];
}

Texture2D ResourceManager::GetTexture(string name)
{
	return Textures[name];
}

ModelData ResourceManager::LoadModelData(string filename, string name) {
	Models[name] = loadModelDataFromFile(filename);

    auto model = Models[name];
    printf("Loaded new model - %s\n", name);
    printf(" Meshes: %d, Lamps: %d ", model.meshes.size(), model.lamps.size());
    int totaltex = 0;
    for (auto mesh : model.meshes) {
        totaltex += mesh.Textures.size();
    }
    printf("Textures: %d\n", totaltex);
    for (auto lamp : model.lamps) {
        printf("Lamp\n");
        printf(" Pos %s\n Col %s\n", glm::to_string(lamp.Position), glm::to_string(lamp.Color));
    }
	return Models[name];
}

ModelData ResourceManager::GetModelData(string name) {
	return Models[name];
}

glm::vec2 ResourceManager::AiToGlm(aiVector2D aiV) {
    return glm::vec2(aiV.x, aiV.y);
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

glm::vec4 ResourceManager::AiToGlm(aiColor3D aiC) {
    return glm::vec4(aiC.r, aiC.g, aiC.b, 1);
}
glm::vec4 ResourceManager::AiToGlm(aiColor4D aiC) {
    return glm::vec4(aiC.r, aiC.g, aiC.b, aiC.a);
}

Texture2D::TextureType ResourceManager::AiToTex2D(aiTextureType aiT) {
    switch (aiT) {
        case aiTextureType_DIFFUSE:
            return Texture2D::TextureType::DIFFUSE;
        case aiTextureType_SPECULAR:
            return Texture2D::TextureType::SPECULAR;
        default:
            throw std::invalid_argument("ResourceManager::AiToTex2D - Texture type is not supported.");
    }
}

void ResourceManager::loadObjectsFromNode(const aiNode* node, const aiScene* scene, glm::mat4 currentTransform, vector<AssimpMesh>* assimpmeshes) {
    double factor;
    if (node->mMetaData != NULL) {
        node->mMetaData->Get("UnitScaleFactor", factor);
        printf("[META]: SF - %f\n", factor);
    }
    

    // Add this node's transform to the stack
    currentTransform = currentTransform * AiToGlm(node->mTransformation);
    // Iterate through each mesh in this node
    for (unsigned int meshIndex=0; meshIndex<node->mNumMeshes; meshIndex++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[meshIndex]];
        // Construct the Mesh struct
        AssimpMesh meshStruct;
        meshStruct.Transform = currentTransform;
        // Add each vertex to the struct
        for (unsigned int vertIndex=0; vertIndex<mesh->mNumVertices; vertIndex++) {
            MeshVertex vertex;
            vertex.Position = AiToGlm(mesh->mVertices[vertIndex]);
            
            if (mesh->mNormals != NULL)
                vertex.Normal = AiToGlm(mesh->mNormals[vertIndex]);

            if (mesh->mTextureCoords[0] != NULL) {
                vertex.TexCoords = (glm::vec2)AiToGlm(mesh->mTextureCoords[0][vertIndex]);
            } else {
                vertex.TexCoords = glm::vec2(0);
            }
            meshStruct.Vertices.push_back(vertex);
        }
        // Add each index inside each face to the struct
        for (unsigned int faceIndex=0; faceIndex<mesh->mNumFaces; faceIndex++) {
            aiFace face = mesh->mFaces[faceIndex];
            // aiProcess_Triangulate should make sure that all faces are triangles

            if (face.mNumIndices != 3) {
                printf("Skipping index %d as it has %d faces.\n", faceIndex, face.mNumIndices);
            } else {
                for (unsigned int i=0; i<face.mNumIndices; i++) {
                    meshStruct.Indices.push_back(face.mIndices[i]);
                }
            }
            
        }
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            aiColor3D color;
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            meshStruct.DiffuseColor = AiToGlm(color);
            material->Get(AI_MATKEY_COLOR_SPECULAR, color);
            meshStruct.SpecularColor = AiToGlm(color);
            material->Get(AI_MATKEY_COLOR_AMBIENT, color);
            meshStruct.AmbientColor = AiToGlm(color);
            material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
            meshStruct.EmissiveColor = AiToGlm(color);
            material->Get(AI_MATKEY_COLOR_TRANSPARENT, color);
            meshStruct.TransparentColor = AiToGlm(color);
            
            meshStruct.Textures = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        }

        assimpmeshes->push_back(meshStruct);
    }

    // Recursively run this function for all of this node's children
    for (unsigned int childIndex=0; childIndex<node->mNumChildren; childIndex++) {
        aiNode* child = node->mChildren[childIndex];
        loadObjectsFromNode(child, scene, currentTransform, assimpmeshes);
    }
}

vector<Texture2D> ResourceManager::loadMaterialTextures(aiMaterial *mat, aiTextureType type) {
    vector<Texture2D> textures;
    for (unsigned int i=0; i<mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        string fullpath = "Textures\\";
        fullpath.append(str.C_Str());
        Texture2D texture = loadTextureFromFile(fullpath.c_str(), false, AiToTex2D(type));
        textures.push_back(texture);
    }
    return textures;
}

ModelData ResourceManager::loadModelDataFromFile(string filename) {
    Assimp::Importer importer;
    ModelData outmodel;
    const aiScene* scene = importer.ReadFile(filename,
        // aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType
    );

    if (!scene) {
        fprintf(stderr, "ASSIMP ERROR - %s\n", importer.GetErrorString());
        return outmodel;
    }

    vector<AssimpMesh> assimpMeshes;
    loadObjectsFromNode(scene->mRootNode, scene, glm::mat4(1.0), &assimpMeshes);

    for (auto mesh : assimpMeshes) {
        vector<MeshVertex> verts;
        vector<GLuint> inds;
        vector<Texture2D> texs;
        Mesh outmesh = Mesh();

        for (auto vert : mesh.Vertices) {
            glm::vec4 vert4 = glm::vec4(vert.Position, 1);
            vert.Position = (glm::vec3)(vert4 * mesh.Transform);
            verts.push_back(vert);
        }   
        for (auto ind : mesh.Indices) {
            inds.push_back(ind);
        }
        for (auto tex : mesh.Textures) {
            texs.push_back(tex);
        }

        // Copy data from struct to Mesh object
        outmesh.Import(verts, inds, texs);
        outmesh.DiffuseColor = mesh.DiffuseColor;

        outmodel.meshes.push_back(outmesh);
    }

    for (unsigned int lampIndex = 0; lampIndex < scene->mNumLights; lampIndex++) {
        ModelLamp lampStruct;
        aiLight* lamp = scene->mLights[lampIndex];
        lampStruct.Color = AiToGlm(lamp->mColorDiffuse);
        lampStruct.Position = AiToGlm(lamp->mPosition);
        outmodel.lamps.push_back(lampStruct);
    }

    return outmodel;
}

void ResourceManager::Clear()
{
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
	Shaders.clear();
	Textures.clear();
	Models.clear();
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

Texture2D ResourceManager::loadTextureFromFile(const GLchar* file, GLboolean alpha, Texture2D::TextureType textype)
{
	Texture2D texture;
	if (alpha) {
		texture.Internal_Format = GL_RGBA;
		texture.Image_Format = GL_RGBA;
	} else {
        texture.Internal_Format = GL_RGB;
        texture.Image_Format = GL_RGB;
    }
	int width, height;
	//unsigned char* image = SOIL_load_image(file, &width, &height, 0, texture.Image_Format == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
	
    int comp = (texture.Image_Format == GL_RGBA ? STBI_rgb_alpha : STBI_rgb);

	unsigned char* image = stbi_load(file, &width, &height, &comp, 0);

	texture.Generate(width, height, image, textype);
	//SOIL_free_image_data(image);
	stbi_image_free(image);
	return texture;
}
