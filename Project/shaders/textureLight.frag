#version 330 core	
	out vec4 FragColor;
	precision mediump float;

	in vec2 v_tex; 
	in vec3 v_frag_coord;
	in vec3 v_normal;

	uniform vec3 u_view_pos;
	uniform sampler2D tex0; 
	
	//In GLSL you can use structures to better organize your code
	//light
	struct Light{
		vec3 light_pos; 
		float ambient_strength; 
		float diffuse_strength; 
		float specular_strength; 
		float constant;
		float linear;
		float quadratic;
	};

	uniform float shininess;

//ADDED
	const int MAX_LIGHTS_NUMBER = 10;
	uniform Light lights[MAX_LIGHTS_NUMBER];
	uniform int n_lights;
	uniform float Ambient;
	uniform vec3 Emitted;

	float specularCalculation(vec3 N, vec3 L, vec3 V ){ 
		vec3 R = reflect (-L,N);  //reflect (-L,N) is  equivalent to //max (2 * dot(N,L) * N - L , 0.0) ;
		float cosTheta = dot(R , V); 
		float spec = pow(max(cosTheta,0.0), 32.0); 
		return lights[0].specular_strength * spec;
	}

	
	//changes by Morgan
//	void main() { 
//		vec3 N = normalize(v_normal);
//		vec3 L = normalize(light.light_pos - v_frag_coord) ; 
//		vec3 V = normalize(u_view_pos - v_frag_coord); 
//		float specular = specularCalculation( N, L, V); 
//		float diffuse = light.diffuse_strength * max(dot(N,L),0.0);
//		float distance = length(light.light_pos - v_frag_coord) + length(u_view_pos - v_frag_coord);
//		float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
//		float light = light.ambient_strength + attenuation * (diffuse + specular); 
//		FragColor = vec4(vec3(texture(tex0, v_tex)) * vec3(light), 1.0); 
//	}


	float calcLight(int i, vec3 N){
		vec3 L = normalize(lights[i].light_pos - v_frag_coord) ; 
		vec3 V = normalize(u_view_pos - v_frag_coord); 
		float specular = specularCalculation( N, L, V); 
		float diffuse = lights[i].diffuse_strength * max(dot(N,L),0.0);
		float distance = length(lights[i].light_pos - v_frag_coord) + length(u_view_pos - v_frag_coord);
		float attenuation = 1 / (lights[i].constant + lights[i].linear * distance + lights[i].quadratic * distance * distance);
		//todo : add some 'emitted' light for the moon
		float light = lights[i].ambient_strength + attenuation * (diffuse + specular);
		if (dot(N,L) <=0){
			light = 0.0;//light.ambient_strength ;
		}
		return light;
	}

	void main() { 
		vec3 N = normalize(v_normal);

		vec3 total_light  = vec3(Ambient) + Emitted; //
		//TODO: initialize with directional light from the sun/moon

		for (int i = 0 ; i < n_lights ; i++){
			total_light += vec3(calcLight(i,N));
		}

		FragColor = vec4(vec3(texture(tex0, v_tex)) * total_light, 1.0); 
	}