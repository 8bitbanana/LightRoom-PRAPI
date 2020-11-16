#pragma once
#include <glm/glm.hpp>

struct LightingInfo {
	glm::vec3 LightPos;
	glm::vec4 LightColor;

	glm::vec4 AmbientColor;
	float AmbientStrength;

	float SpecularStrength;

	glm::vec3 ViewPos;
};