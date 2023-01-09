#version 330 core
out vec4 FragColor;

precision mediump float;
in vec2 v_tex; 
uniform sampler2D tex0; 

void main()
{ 
	FragColor = vec4(vec3(texture(tex0, v_tex)),1.0);
}