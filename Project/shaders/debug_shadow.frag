#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;

void main(){
	float depth_val = texture(depthMap, TexCoords).r;
	FragColor = vec4(vec3(depth_val), 1.0);
}
