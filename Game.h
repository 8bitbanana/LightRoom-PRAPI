#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Code/LightingInfo.h"

class Game
{
public:
	GLboolean Keys[1024];
	GLuint Width, Height;
	glm::vec2 Mouse;

	Game(GLuint width, GLuint height);
	~Game();

	void Init();
	void Update(GLfloat dt);
	void Draw();
	void ResizeEvent(GLfloat width, GLfloat height);
private:
	void CalculateCamera();
	void CalculateLighting();

	float dt;

	glm::vec3 CameraPos;
	glm::vec3 CameraRot;
	//glm::tquat<float> CameraRot;
	glm::highp_mat4 CurrentProjection;
	glm::highp_mat4 CurrentView;
};