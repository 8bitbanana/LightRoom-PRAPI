#pragma once
#include <glm/glm.hpp>

#define MAX_LIGHTS 8

struct LightingInfo {
	glm::vec3 LightPos[MAX_LIGHTS];
	glm::vec4 LightColor[MAX_LIGHTS];
	float SpecularStrength[MAX_LIGHTS];
	bool LightActive[MAX_LIGHTS];

	glm::vec4 AmbientColor;
	float AmbientStrength;
	glm::vec3 ViewPos;
};