#version 400 core
	out vec4 FragColor;
	precision mediump float; 

	in vec3 v_frag_coord; 
	in vec3 v_normal;

	uniform vec3 u_view_pos; 
	uniform float refractionIndice;
	uniform samplerCube cubemapSampler; 
	

	void main() 
	{ 
		float ratio = 1.00 / refractionIndice;
		vec3 N = normalize(v_normal);
		vec3 V = normalize(u_view_pos - v_frag_coord); 
		vec3 R = refract(-V,N,ratio); 
		FragColor = texture(cubemapSampler,R);
		//FragColor = vec4 (1.0, 0.0, 0.0, 1.0);
	} 