#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

uniform vec4 color;

#define MAX_LIGHTS 8
#define MAX_TEXTURES 8

uniform bool lightActive[MAX_LIGHTS];
uniform vec4 lightColor[MAX_LIGHTS];
uniform vec3 lightPos[MAX_LIGHTS];
uniform float specularStrength[MAX_LIGHTS];
uniform vec4 ambientColor;
uniform float ambientStrength;
uniform vec3 viewPos;

uniform bool diffuseActive[MAX_TEXTURES];
uniform bool specularActive[MAX_TEXTURES];

uniform sampler2D texture_diffuse[MAX_TEXTURES];
uniform sampler2D texture_specular[MAX_TEXTURES];

void main() {
	vec3 norm = normalize(Normal);

	vec3 ambient = ambientStrength * vec3(ambientColor);
	vec3 lighting = ambient;

	for (int i=0; i<MAX_LIGHTS; i++) {
		if (!lightActive[i]) {
			continue;
		}

		vec3 lightDir = normalize(lightPos[i] - FragPos);

		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * vec3(lightColor[i]);
		lighting += diffuse;

		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = specularStrength[i] * spec * vec3(lightColor[i]);
		lighting += specular;
	}

	if (diffuseActive[0])
		FragColor = vec4(lighting, 1) * texture(texture_diffuse[0], TexCoord);
	else
		FragColor = vec4(lighting, 1) * color;
}