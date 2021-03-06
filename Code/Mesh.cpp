#include "Mesh.h"

#include <string>
#include <map>
#include <stdexcept>

using std::string;
using std::map;

Mesh::Mesh() {

}

void Mesh::Import(vector<MeshVertex> vertices, vector<GLuint> indices, vector<Texture2D> textures) {
	Vertices = vertices;
	Indices = indices;
	Textures = textures;

    glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	// positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)0);

	// normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Normal));

	// texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, TexCoords));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader, LightingInfo& lighting) {

	shader.SetVector4f("color", &DiffuseColor);

	shader.SetBool("lightActive", (GLboolean *)&lighting.LightActive[0], MAX_LIGHTS);
	shader.SetVector4f("lightColor", &lighting.LightColor[0], MAX_LIGHTS);
	shader.SetVector3f("lightPos", &lighting.LightPos[0], MAX_LIGHTS);
	shader.SetFloat("specularStrength", &lighting.SpecularStrength[0], MAX_LIGHTS);

	shader.SetVector4f("ambientColor", &lighting.AmbientColor);
	shader.SetFloat("ambientStrength", &lighting.AmbientStrength);
	shader.SetVector3f("viewPos", &lighting.ViewPos);

	map<Texture2D::TextureType, int> texCounts;
	
	GLboolean diffuseActive[MAX_TEXTURES] = {GL_FALSE};
	GLboolean specularActive[MAX_TEXTURES] = {GL_FALSE};
	for (GLint i=0; i<Textures.size(); i++) {
		Texture2D* tex = &Textures[i];
		glActiveTexture(GL_TEXTURE0 + i);
		// Maps auto initialise variables
		//if (texCounts.count(tex->Type) == 0) texCounts[tex->Type] = 0;
		int number = texCounts[tex->Type]++;
		string numberStr = std::to_string(number);
		string name = tex->GetTypeStr();
		string fullname;
		switch (tex->Type) {
			case Texture2D::TextureType::DIFFUSE:
				fullname = "texture_diffuse[" + numberStr + "]";
				diffuseActive[number] = GL_TRUE;
				break;
			case Texture2D::TextureType::SPECULAR:
				fullname = "texture_specular[" + numberStr + "]";
				specularActive[number] = GL_TRUE;
				break;
			default:
				throw new std::runtime_error("Unknown texture type.");
		}
		//printf("%s\n", fullname.c_str());
		// tell shader to use GL_TEXTURE0+i for this texture
		shader.SetInteger(fullname.c_str(), &i);
		// bind texture to GL_TEXTURE0+i
		tex->Bind();
	}
	shader.SetBool("diffuseActive", &diffuseActive[0], MAX_TEXTURES);
	shader.SetBool("specularActive", &specularActive[0], MAX_TEXTURES);

    glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

	//glActiveTexture(GL_TEXTURE0);
}