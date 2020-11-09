#version 330 core
out vec4 FragColor;

uniform vec4 color;

in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular0;
uniform sampler2D texture_specular1;

void main() {
	if (Normal.x == 0) {

	}
	FragColor = color * texture(texture_diffuse0, TexCoord);
}