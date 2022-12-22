#version 330 core
	in vec3 position; 


	uniform mat4 M; 
	uniform mat4 V; 
	uniform mat4 P; 
	uniform mat4 R;
	void main()
	{ 
		gl_Position = P*V*R*M*vec4(position, 1.0);
	}