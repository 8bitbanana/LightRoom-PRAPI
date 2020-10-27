#include "Game.h"
#include "Code\\ResourceManager.h"
#include "Code\\Model.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>

const glm::vec3 cameraForward = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);

Model* object;

Game::Game(GLuint width, GLuint height)
	: Width(width), Height(height)
{
	CameraPos = glm::vec3(0, 0, 5.0f);
	CameraRot = glm::vec3(0, -90.0f, 0);
}

Game::~Game()
{
	delete object;
}

void Game::Init()
{
	ResourceManager::LoadShader("Shaders/baseproj.vert", "Shaders/baseproj.frag", nullptr, "baseproj");
	ResourceManager::LoadMesh("Models/rook.obj", "rook");
	CurrentProjection = glm::perspective(glm::radians(60.0f), float(Width) / Height, 0.1f, 100.0f);

	object = new Model("rook", glm::vec3(0), glm::vec3(0), glm::vec3(1));
}

void Game::Update(GLfloat dt)
{
	UpdateCamera();
}

void Game::ProcessInput(GLfloat dt)
{
	
}

void Game::UpdateCamera() {
	CurrentView = glm::mat4(1.0);
	CurrentView = glm::lookAt(CameraPos, glm::vec3(0), cameraUp);
	// glm::translate(CurrentView, CameraPos);
	// glm::quat rot = glm::quat(CameraRot);
	// CurrentView *= glm::toMat4(rot);
}

void Game::Draw()
{
	object->Draw(CurrentProjection, CurrentView);
}

void Game::ResizeEvent(GLfloat width, GLfloat height)
{
	Width = width;
	Height = height;
}