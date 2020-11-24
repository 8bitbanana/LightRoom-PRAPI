#pragma once

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


// General purpsoe shader object. Compiles from file, generates
// compile/link-time error messages and hosts several utility 
// functions for easy management.
class Shader
{
public:
	// State
	GLuint ID;
	// Constructor
	Shader() { }
	// Sets the current shader as active
	Shader& Use();
	// Compiles the shader from given source code
	void    Compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource = nullptr); // Note: geometry source code is optional 
	// Utility functions
	void    SetBool(const GLchar* name, GLboolean* value, GLsizei = 1, GLboolean useShader = false);
	void    SetFloat(const GLchar* name, GLfloat* value, GLsizei = 1, GLboolean useShader = false);
	void    SetInteger(const GLchar* name, GLint* value, GLsizei = 1, GLboolean useShader = false);
	void    SetVector2f(const GLchar* name, glm::vec2* value, GLsizei = 1, GLboolean useShader = false);
	void    SetVector3f(const GLchar* name, glm::vec3* value, GLsizei = 1, GLboolean useShader = false);
	void    SetVector4f(const GLchar* name, glm::vec4* value, GLsizei = 1, GLboolean useShader = false);
	void    SetMatrix4(const GLchar* name, glm::mat4* matrix, GLsizei = 1, GLboolean useShader = false);
private:
	// Checks if compilation or linking failed and if so, print the error logs
	void    checkCompileErrors(GLuint object, std::string type);
};