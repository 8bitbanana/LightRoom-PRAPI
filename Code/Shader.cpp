#include "Shader.h"

#include <iostream>

Shader& Shader::Use()
{
	glUseProgram(this->ID);
	return *this;
}

void Shader::Compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource)
{
	GLuint sVertex, sFragment, gShader;
	auto a = 1;
	// Vertex Shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, "VERTEX");
	// Fragment Shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");
	// If geometry shader source code is given, also compile geometry shader
	if (geometrySource != nullptr)
	{
		gShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShader, 1, &geometrySource, NULL);
		glCompileShader(gShader);
		checkCompileErrors(gShader, "GEOMETRY");
	}
	// Shader Program
	this->ID = glCreateProgram();
	glAttachShader(this->ID, sVertex);
	glAttachShader(this->ID, sFragment);
	if (geometrySource != nullptr)
		glAttachShader(this->ID, gShader);
	glLinkProgram(this->ID);
	checkCompileErrors(this->ID, "PROGRAM");
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (geometrySource != nullptr)
		glDeleteShader(gShader);
}

void Shader::SetBool(const GLchar* name, GLboolean* value, GLsizei count, GLboolean useShader)
{
	auto BoolArray = new GLint[count];
	for (unsigned int i=0; i<count; i++) {
		BoolArray[i] = (GLint)value[i];
	}
	if (useShader)
		this->Use();
	glUniform1iv(glGetUniformLocation(this->ID, name), count, BoolArray);
	delete[] BoolArray;
}

void Shader::SetFloat(const GLchar* name, GLfloat* value, GLsizei count, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform1fv(glGetUniformLocation(this->ID, name), count, value);
}
void Shader::SetInteger(const GLchar* name, GLint* value, GLsizei count, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform1iv(glGetUniformLocation(this->ID, name), count, value);
}
void Shader::SetVector2f(const GLchar* name, glm::vec2* value, GLsizei count, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform2fv(glGetUniformLocation(this->ID, name), count, (GLfloat*)value);
}
void Shader::SetVector3f(const GLchar* name, glm::vec3* value, GLsizei count, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform3fv(glGetUniformLocation(this->ID, name), count, (GLfloat*)value);
}
void Shader::SetVector4f(const GLchar* name, glm::vec4* value, GLsizei count, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniform4fv(glGetUniformLocation(this->ID, name), count, (GLfloat*)value);
}
void Shader::SetMatrix4(const GLchar* name, glm::mat4* matrix, GLsizei count, GLboolean useShader)
{
	if (useShader)
		this->Use();
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name), count, GL_FALSE, (GLfloat*)matrix);
}


void Shader::checkCompileErrors(GLuint object, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
	else
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
}