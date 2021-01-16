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
	//ResourceManager::LoadModelData("Models/ball_mars.obj", "ball");
	ResourceManager::LoadModelData("Models/cube-light.obj", "cube-light");
	CurrentProjection = glm::perspective(glm::radians(60.0f), float(Width) / Height, 0.1f, 100.0f);

	//objects.push_back(new Model("ball"));
	auto examplemodel = new Model("cube-light");
	objects.push_back(examplemodel);
}

void Game::Update(GLfloat dt)
{
	this->dt = dt;

	//CalculateLighting();
	CalculateCamera();

	for (auto object : objects) {
		object->Update(dt);
	}
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
		object->Draw(CurrentProjection, CurrentView, CameraPos);
	}
}

void Game::ResizeEvent(GLfloat width, GLfloat height)
{
	Width = width;
	Height = height;
}