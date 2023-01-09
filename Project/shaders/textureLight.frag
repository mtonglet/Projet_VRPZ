#version 330 core	
	out vec4 FragColor;
	precision mediump float;

	in vec2 v_tex; 
	in vec3 v_frag_coord;
	in vec3 v_normal;

	uniform vec3 u_view_pos;
	uniform sampler2D tex0;
	uniform bool lampsActivated;
	
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
	uniform vec3 emitted;

	float specularCalculation(vec3 N, vec3 L, vec3 V , int i){ 
		vec3 R = reflect (-L,N);  //reflect (-L,N) is  equivalent to //max (2 * dot(N,L) * N - L , 0.0) ;
		float cosTheta = dot(R , V); 
		float spec = pow(max(cosTheta,0.0), 32.0); 
		return lights[i].specular_strength * spec;
	}

	float calcDirLight(vec3 N){
		vec3 L = normalize(lights[0].light_pos); 
		vec3 V = normalize(u_view_pos - v_frag_coord); 
		float specular = specularCalculation( N, L, V, 0); 
		float diffuse = lights[0].diffuse_strength * max(dot(N,L),0.0);
		float light = lights[0].ambient_strength + diffuse + specular;
		if (dot(N,L) <= 0){
			light = lights[0].ambient_strength ;//+ diffuse + specular; //
		}

		return light;
	}

	float calcLight(int i, vec3 N){
		vec3 L = normalize(lights[i].light_pos - v_frag_coord) ; 
		vec3 V = normalize(u_view_pos - v_frag_coord); 
		float specular = specularCalculation( N, L, V, i); 
		float diffuse = lights[i].diffuse_strength * max(dot(N,L),0.0);
		float distance = length(lights[i].light_pos - v_frag_coord) + length(u_view_pos - v_frag_coord);
		float attenuation = 1 / (lights[i].constant + lights[i].linear * distance + lights[i].quadratic * distance * distance);
		//todo : add some 'emitted' light for the moon
		//float light = lights[i].ambient_strength + attenuation * (diffuse + specular);
		float light =  attenuation * (diffuse + specular);
		if (dot(N,L) <=0){
			light = 0.0;
		}
		return light;
	}

	void main() { 
		vec3 N = normalize(v_normal);
		float dirli = calcDirLight(N);
		vec3 total_light = vec3(dirli);

		total_light  +=  emitted;

		if (lampsActivated){
			for (int i = 1 ; i < n_lights ; i++){
				total_light += vec3(calcLight(i,N));
			}
		}

		FragColor = vec4(vec3(texture(tex0, v_tex)) * total_light, 1.0); 
	}