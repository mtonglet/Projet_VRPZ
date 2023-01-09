#version 330 core
layout (location=0) in vec3 pos;

uniform mat4 lightProj; 
uniform mat4 model; 

void main()
{ 
	gl_Position = lightProj * M * vec4(pos,1.0);
}