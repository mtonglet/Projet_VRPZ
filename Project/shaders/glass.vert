#version 330 core
	in vec3 position; 
	in vec2 tex_coord;
	in vec3 normal; 

	out vec2 TexCoords;

	uniform mat4 M; 
	uniform mat4 V; 
	uniform mat4 P; 
	
	void main()
	{ 
		gl_Position = P*V*M*vec4(position, 1.0);
		//TexCoords = (P*V*M*vec4(position, 1.0)).xy/2+0.5;
		TexCoords = (gl_Position.xy / gl_Position.w +1.0)/2.0;///2+0.5;
		//TexCoords = tex_coord; 
		if(tex_coord.x > 5 && normal.x > 5){
		TexCoords  = vec2(1.0);
		}
	}

