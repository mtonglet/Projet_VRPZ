#version 330 core
	in vec3 position; 

	in vec2 tex_coord; 
	out vec2 v_tex; 

	uniform mat4 M; 
	uniform mat4 V; 
	uniform mat4 P; 
	void main()
	{ 
		gl_Position = P*V*M*vec4(position, 1.0);
		v_tex = tex_coord; 
	}