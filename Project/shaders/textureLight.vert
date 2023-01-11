#version 330 core
	in vec3 position; 
	in vec2 tex_coord; 
	in vec3 normal;

	out vec2 v_tex; 
	out vec3 v_frag_coord; 
	out vec3 v_normal;
	out vec4 frag_pos_light;

	uniform mat4 M; 
	uniform mat4 R;
	uniform mat4 V; 
	uniform mat4 P; 
	uniform mat4 itM;
	uniform mat4 dir_light_proj;

	void main()
	{ 
		//c'est quoi R ?!
		vec4 frag_coord = R*M*vec4(position, 1.0); 

		v_tex = tex_coord; 
//		v_frag_coord = frag_coord.xyz; 
		v_frag_coord = vec3(frag_coord);//.xyz; 
		v_normal = transpose(inverse(mat3(M))) * normal;
//		v_normal = vec3(itM * vec4(normal, 1.0));
		frag_pos_light = dir_light_proj * M * vec4(position,1.0);


		gl_Position = P*V*frag_coord;

//		frag_pos_light = dir_light_proj * vec4(position,1.0);//not position
//		v_normal = vec3(itM * vec4(normal, 1.0));
//		v_frag_coord = vec3(M * vec4(position, 1.0));
//		v_tex = tex_coord;
//		dir_light_proj = lightSpaceMatrix * vec4(v_frag_coord, 1.0);
//		gl_Position = P * V * vec4(v_frag_coord, 1.0);


	}