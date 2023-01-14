#version 330 core	
	precision mediump float;
	const int MAX_LIGHTS_NUMBER = 10;
	const float PI = 3.14159265358979f;
	in vec2 v_tex; 
	in vec3 v_frag_coord;
	in vec3 v_normal;
	in vec4 frag_pos_light;

	out vec4 FragColor;

	struct LightParams{
		float ambient_strength; 
		float diffuse_strength; 
		float specular_strength; 
		float constant;
		float linear;
		float quadratic;
	};


	uniform bool lampsActivated;
	uniform int n_lights;
	uniform float shininess;
	uniform vec3 u_view_pos;
	uniform vec3 emitted;
	uniform vec3 lights[MAX_LIGHTS_NUMBER];
	uniform sampler2D tex0;
	uniform sampler2D shadow_map;
	uniform samplerCube shadow_cube_map[MAX_LIGHTS_NUMBER];
	uniform float far_back_cube;

	uniform LightParams spot_light_param;
	uniform LightParams dir_light_param;
	uniform LightParams point_light_param;
	uniform float lampRefl;
	
	LightParams light_param;
	

	float specularCalculation(vec3 N, vec3 L, vec3 V , int i){ 
		float spec = 0.0f;
		if (dot(N,L)>0){
			vec3 R = reflect (-L,N);  //reflect (-L,N) is  equivalent to //max (2 * dot(N,L) * N - L , 0.0) ;
			float cosTheta = dot(R , V); 
			spec = pow(max(cosTheta,0.0), 32.0); 
		}
		return light_param.specular_strength * spec;
	}

	float shadowCalculation(float dotNL){
		float shad = 0.0f;
		vec3 light_coords = frag_pos_light.xyz/frag_pos_light.w;

		int rad = 2; //for kernel dimension

		if (light_coords.z <= 1.0f){
			light_coords = (light_coords + 1.0f) / 2.0f;

			float current_depth = light_coords.z;
			float bias = max(0.00005f, (1.0f-dotNL) * 0.0005f);

			vec2 pixel_dims = 1.0f / textureSize(shadow_map, 0);
			for(int x=-rad ; x<=rad ; x++){
				for(int y=-rad ; y<=rad ; y++){
					float closest_depth = texture(shadow_map,light_coords.xy + pixel_dims*vec2(x,y)).x;
					if (current_depth > closest_depth + bias){
						shad += 1.0f;
					}
				}			
			}
		}
		shad = shad / ((2*rad+1)*(2*rad+1));
		return shad;
	}


	float shadowCubeCalculation(int i,float dotNL){
		float shadow = 0.0f;
		vec3 light_to_frag = v_frag_coord - lights[i];
		float cur_depth = length(light_to_frag);
		float bias = max(0.005f, (1.0f-dotNL) * 0.5f);

		float closest_depth = texture(shadow_cube_map[i-1],light_to_frag).r;
		closest_depth *= far_back_cube;
		if (cur_depth > closest_depth + bias){
			shadow += 1.0f;
		}
		
		return shadow;
	}


	float calcDirLight(vec3 N){
		light_param=dir_light_param;
		vec3 li_coords = frag_pos_light.xyz/ frag_pos_light.w; 

		vec3 L = normalize(lights[0]); 
		vec3 V = normalize(u_view_pos - v_frag_coord); 
		float specular = specularCalculation( N, L, V, 0); 
		float diffuse = light_param.diffuse_strength * max(dot(N,L),0.0);

		float shad = shadowCalculation(dot(N,L));      //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		
		float light = light_param.ambient_strength + (diffuse + specular)*(1.0f - shad);

		if (dot(N,L) <= 0){
			light = light_param.ambient_strength ;//+ diffuse + specular; //
		}

		return light;
	}

	float calcPointLight(int i, vec3 N){
		light_param=point_light_param;
		vec3 L = normalize(lights[i] - v_frag_coord) ; 
		vec3 V = normalize(u_view_pos - v_frag_coord);
		float ambiant = light_param.ambient_strength;
		float specular = specularCalculation( N, L, V, i); 
		float diffuse = light_param.diffuse_strength * max(dot(N,L),0.0);
		float distance = length(lights[i] - v_frag_coord) + length(u_view_pos - v_frag_coord);
		float attenuation = 1 / (light_param.constant + light_param.linear * distance + light_param.quadratic * distance * distance);

		float shad = shadowCubeCalculation(i, dot(N,L));

		float light =  ambiant + attenuation * (diffuse + specular)*(1.0f-shad);
		if (dot(N,L) <=0){
			light = ambiant;
		}

		return light;
	}

	float calcSpotLight(int i, vec3 N){
		light_param=spot_light_param;
		float innerAngle = 1.0f; //in cosinus value
		float outerAngle = 0.9f;

		vec3 L = normalize(lights[i] - v_frag_coord) ;
		vec3 V = normalize(u_view_pos - v_frag_coord);
		float ambiant = 0; 
		float specular = specularCalculation( N, L, V, i); 
		float diffuse = light_param.diffuse_strength * max(dot(N,L),0.0);
		float distance = length(lights[i] - v_frag_coord) + length(u_view_pos - v_frag_coord);
		float attenuation = 1 / (light_param.constant + light_param.linear * distance + light_param.quadratic * distance * distance);

//		float angle = dot(vec3(0.0f, -1.0f, 0.0f), - L);
		//to make spotlights pointing towards the origin
		vec3 lightDir = normalize(-lights[i])+vec3(0.0f,1.0f,0.0f);
		float angle = dot(normalize(lightDir), - L);

		float intensity = clamp((angle - outerAngle)/(innerAngle - outerAngle), 0.0f, 1.0f);

		float light =  ambiant + attenuation * intensity * (diffuse + specular);
		if (dot(N,L) <=0){
			light = ambiant;
		}
		return light;	
	}

	void main() { 
		vec3 N = normalize(v_normal);


		vec3 total_light = vec3(calcDirLight(N));

		total_light  +=  emitted;

		for (int i = 1 ; i < n_lights ; i++){
			if (lampsActivated || i==1){
				total_light += vec3(calcPointLight(i,N));
//				total_light += vec3(calcSpotLight(i,N));
			}
		}

		FragColor = vec4(vec3(texture(tex0, v_tex)) * total_light, 1.0); 
	}