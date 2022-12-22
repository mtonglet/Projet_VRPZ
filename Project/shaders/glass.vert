#version 330 core
	in vec3 position; 
	in vec2 tex_coords;
	in vec3 normal; 

	out vec2 TexCoords;

	uniform mat4 M; 
	uniform mat4 V; 
	uniform mat4 P; 
	
	void main()
	{ 
		gl_Position = P*V*M*vec4(position, 1.0);
		TexCoords = (gl_Position.xy)/2+0.5;
	}

