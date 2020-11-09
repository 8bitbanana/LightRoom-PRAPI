#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec2 TexCoord;

uniform mat4 pvm;

void main() {
	gl_Position = pvm * vec4(aPos, 1);
    Normal = aNormal;
    TexCoord = aTexCoord;
}