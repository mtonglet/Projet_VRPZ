#version 330 core	
	precision mediump float;
	const int MAX_LIGHTS_NUMBER = 10;

	in vec2 v_tex; 
	in vec3 v_frag_coord;
	in vec3 v_normal;
	in vec4 frag_pos_light;

	out vec4 FragColor;

	struct Light{
		vec3 light_pos; 
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
	uniform Light lights[MAX_LIGHTS_NUMBER];
	uniform sampler2D shadow_map;
	uniform sampler2D tex0;
	

	float specularCalculation(vec3 N, vec3 L, vec3 V , int i){ 
		vec3 R = reflect (-L,N);  //reflect (-L,N) is  equivalent to //max (2 * dot(N,L) * N - L , 0.0) ;
		float cosTheta = dot(R , V); 
		float spec = pow(max(cosTheta,0.0), 32.0); 
		return lights[i].specular_strength * spec;
	}

//	float shadowCalculation(){
//		float shad = 0.0f;
		//persp. division to get into the clip space
//		vec3 light_coords = frag_pos_light.xyz/frag_pos_light.w;
//
		//where inside the frustrum of the orthographic proj
//		if (light_coords.z <= 1.0f){
//			light_coords = (light_coords + 1.0f) / 2.0f;
//
//			float closest_depth = texture(shadow_map,light_coords.xy).r;
//			float current_depth = light_coords.z;
//			float bias = 0.005f;
//			if (current_depth > closest_depth + bias){
//				shad = 1.0f;
//			}
//		}
//		return shad;
//	}

	float otherShadCalc(float dotNL){
		vec3 pos = frag_pos_light.xyz * 0.5 + 0.5;
		if(pos.z > 1.0){
			pos.z = 1.0;
		}
		float depth = texture(shadow_map,pos.xy).r;
		float bias = max(0.05*(1-dotNL),0.005);
		return (depth + bias) < pos.z ? 0.01 : 1.0;
	}

	float calcDirLight(vec3 N){
		vec3 li_coords = frag_pos_light.xyz/ frag_pos_light.w; 

		vec3 L = normalize(lights[0].light_pos); 
		vec3 V = normalize(u_view_pos - v_frag_coord); 
		float specular = specularCalculation( N, L, V, 0); 
		float diffuse = lights[0].diffuse_strength * max(dot(N,L),0.0);

		float shad = otherShadCalc(dot(N,L));
		float light = lights[0].ambient_strength + (diffuse + specular)*shad;
		if (dot(N,L) <= 0){
			//light = lights[0].ambient_strength ;//+ diffuse + specular; //
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
		vec3 color = vec3(texture(tex0, v_tex)).rgb;
		FragColor = vec4(color * total_light, 1.0); 
	}