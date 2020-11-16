#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

uniform vec4 color;
uniform vec4 lightColor;
uniform vec3 lightPos;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular0;
uniform sampler2D texture_specular1;

void main() {
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(lightColor);

	vec4 lighting = vec4(diffuse, 1); // * ambient * spec * etc

	FragColor = lighting * color * texture(texture_diffuse0, TexCoord);
}