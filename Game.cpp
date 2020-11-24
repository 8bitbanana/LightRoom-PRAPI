#include "Game.h"
#include "Code\\ResourceManager.h"
#include "Code\\Model.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>

const glm::vec3 FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 RIGHT = glm::vec3(1.0f, 0.0f, 0.0f);
const float MOUSE_SENS = 45.0f;
const float MOVE_SPEED = 10.0f;

vector<Model*> objects;

Game::Game(GLuint width, GLuint height)
	: Width(width), Height(height)
{
	CameraPos = glm::vec3(0, 0, 5.0f);
	CameraRot = glm::vec3(0, 0, 0);
}

Game::~Game()
{
	for (auto object : objects) {
		delete object;
	}
}

void Game::Init()
{
	ResourceManager::LoadShader("Shaders/baseproj.vert", "Shaders/baseproj.frag", nullptr, "baseproj");
	ResourceManager::LoadShader("Shaders/material.vert", "Shaders/material.frag", nullptr, "material");
	ResourceManager::LoadMeshes("Models/ball_mars.obj", "ball");
	CurrentProjection = glm::perspective(glm::radians(60.0f), float(Width) / Height, 0.1f, 100.0f);

	objects.push_back(new Model("ball", glm::vec3(0), glm::vec3(0), glm::vec3(1)));

	for (int i=0; i<MAX_LIGHTS; i++)
		lighting.LightActive[i] = false;

	lighting.LightColor[0] = glm::vec4(0,1,0,1);
	lighting.LightPos[0] = glm::vec3(1.2f, 1.0f, 2.0f);
	lighting.LightActive[0] = true;
	lighting.SpecularStrength[0] = 0.5f;

	lighting.LightColor[1] = glm::vec4(1,0,0,1);
	lighting.LightPos[1] = glm::vec3(1.2f, 1.0f, 2.0f);
	lighting.LightActive[1] = true;
	lighting.SpecularStrength[1] = 0.4f;

	lighting.LightColor[2] = glm::vec4(0.2,0.2,1,1);
	lighting.LightPos[2] = glm::vec3(1.2f, 1.0f, 2.0f);
	lighting.LightActive[2] = true;
	lighting.SpecularStrength[2] = 1.0f;

	lighting.AmbientColor = glm::vec4(1);
	lighting.AmbientStrength = 0.4f;
}

void Game::Update(GLfloat dt)
{
	this->dt = dt;

	CalculateLighting();
	CalculateCamera();

	for (auto object : objects) {
		object->Update(dt);
	}
}

void Game::CalculateLighting() {
	lighting.LightPos[0] = glm::vec3(glm::mat4_cast(glm::angleAxis(5*dt, UP)) * glm::vec4(lighting.LightPos[0], 1));
	lighting.LightPos[1] = glm::vec3(glm::mat4_cast(glm::angleAxis(-3*dt, UP)) * glm::vec4(lighting.LightPos[1], 1));
	lighting.LightPos[2] = glm::vec3(glm::mat4_cast(glm::angleAxis(8*dt, UP)) * glm::vec4(lighting.LightPos[2], 1));
	lighting.ViewPos = CameraPos;
}

void Game::CalculateCamera() {
	CameraRot += glm::vec3(Mouse.y, -Mouse.x, 0) * dt * MOUSE_SENS;
	if (CameraRot.x > 89.0f)  CameraRot.x = 89.0f;
	if (CameraRot.x < -89.0f) CameraRot.x = -89.0f;

	glm::vec3 direction = FORWARD;
	glm::quat rot = glm::quat(glm::radians(CameraRot));
	direction = glm::rotate(rot, direction);
	direction = glm::normalize(direction);

	// Rotation without X axis, for use with movement
	glm::quat rotNoX = glm::quat(glm::radians(glm::vec3(
		0,
		CameraRot.y,
		CameraRot.z
	)));

	glm::vec3 moveVector(0,0,0);
	if (Keys[GLFW_KEY_W])
		moveVector += FORWARD;
	if (Keys[GLFW_KEY_A])
		moveVector -= RIGHT;
	if (Keys[GLFW_KEY_S])
		moveVector -= FORWARD;
	if (Keys[GLFW_KEY_D])
		moveVector += RIGHT;
	if (Keys[GLFW_KEY_SPACE])
		moveVector += UP;
	if (Keys[GLFW_KEY_LEFT_CONTROL])
		moveVector -= UP;
	moveVector *= dt * MOVE_SPEED;
	CameraPos += glm::rotate(rotNoX, moveVector);

	CurrentView = glm::mat4(1.0);
	CurrentView = glm::lookAt(CameraPos, CameraPos + direction, UP);
}

void Game::Draw()
{
	for (auto object : objects) {
		object->Draw(CurrentProjection, CurrentView, lighting);
	}
}

void Game::ResizeEvent(GLfloat width, GLfloat height)
{
	Width = width;
	Height = height;
}