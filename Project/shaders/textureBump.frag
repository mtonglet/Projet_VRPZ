#version 330 core
#define MAX_LIGHTS_NUMBER 10
	out vec4 FragColor;
	precision mediump float;


	in vec3 Normal;
	in vec2 texCoord; 
	in vec3 fragCoord;
	in vec3 u_view_pos;
	in vec3 lights[10];
	in vec4 u_frag_pos_light;


	uniform int n_lights;


	//In GLSL you can use structures to better organize your code
	//light
	struct LightParams{
		float ambient_strength; 
		float diffuse_strength; 
		float specular_strength; 
		float constant;
		float linear;
		float quadratic;
	};

	uniform float shininess; 
	uniform LightParams spot_light_param;
	uniform LightParams dir_light_param;
	uniform LightParams point_light_param;
	uniform LightParams light_param;

	uniform bool lampsActivated;
	uniform sampler2D tex0; 
	uniform sampler2D normal0;
	uniform sampler2D shadow_map;


	
//GOOD ONE THAT WORKS !!
//	float specularCalculation(vec3 N, vec3 L, vec3 V ){ 
//		vec3 R = reflect (-L,N); 
//		float cosTheta = dot(R , V); 
//		float spec = pow(max(cosTheta,0.0), 32.0); 
//		return light.specular_strength * spec;
//	}
//
//	void main() { 
//		vec3 N = normalize(texture(normal0, texCoord).xyz * 2.0f - 1.0f);
//
//		vec3 L = normalize(lightPos - fragCoord) ; 
//		vec3 V = normalize(u_view_pos - fragCoord); 
//		float specular = specularCalculation( N, L, V); 
//		float diffuse = light.diffuse_strength * max(dot(N,L),0.0);
//		float distance = length(lightPos - fragCoord) + length(u_view_pos - fragCoord);
//		float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
//		float light = light.ambient_strength ;
//		if (dot(N,L) >=0){
//		light += attenuation * (diffuse + specular) ;
//		}
//
//		FragColor = vec4(vec3(texture(tex0, texCoord)) * vec3(light), 1.0); 
//	}



	float specularCalculation(vec3 N, vec3 L, vec3 V , int i){ 
		vec3 R = reflect (-L,N); 
		float cosTheta = dot(R , V); 
		float spec = pow(max(cosTheta,0.0), 32.0); 
		return light_param.specular_strength * spec;
	}


	
	float shadowCalculation(float dotNL){
		float shad = 0.0f;
		vec3 light_coords = u_frag_pos_light.xyz/u_frag_pos_light.w;

		int rad = 2; //at least 1 -> 

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

	float calcDirLight(vec3 N){
		vec3 L = normalize(lights[0]); 
		vec3 V = normalize(u_view_pos - fragCoord); 
		float specular = specularCalculation( N, L, V, 0); 
		float diffuse = light_param.diffuse_strength * max(dot(N,L),0.0);
		float shad = shadowCalculation(dot(N,L));

		float light = light_param.ambient_strength + (diffuse + specular)*(1.0f - shad);
		if (dot(N,L) <= 0){
			light = light_param.ambient_strength ; //
		}
		return light;
	}

	float calcPointLight(int i, vec3 N){
		vec3 L = normalize(lights[i] - fragCoord) ; 
		vec3 V = normalize(u_view_pos - fragCoord);
		float ambiant = light_param.ambient_strength;
		float specular = specularCalculation( N, L, V, i); 
		float diffuse = light_param.diffuse_strength * max(dot(N,L),0.0);
		float distance = length(lights[i] - fragCoord) + length(u_view_pos - fragCoord);
		float attenuation = 1 / (light_param.constant + light_param.linear * distance + light_param.quadratic * distance * distance);
		float light =  ambiant + attenuation * (diffuse + specular);
		if (dot(N,L) <=0){
			light = ambiant;
		}
		return light;
	}

	float calcSpotLight(int i, vec3 N){
		float innerAngle = 1.0f; //in cosinus value
		float outerAngle = 0.9f;

		vec3 L = normalize(lights[i] - fragCoord) ; 
		vec3 V = normalize(u_view_pos - fragCoord);
		float ambiant = 0;//light_param.ambient_strength;
		float specular = specularCalculation( N, L, V, i); 
		float diffuse = light_param.diffuse_strength * max(dot(N,L),0.0);
		float distance = length(lights[i] - fragCoord) + length(u_view_pos - fragCoord);
		float attenuation = 1 / (light_param.constant + light_param.linear * distance + light_param.quadratic * distance * distance);

		float angle = dot(vec3(0.0f, -1.0f, 0.0f), - L);
		float intensity = clamp((angle - outerAngle)/(innerAngle - outerAngle), 0.0f, 1.0f);

		float light =  ambiant + attenuation * intensity * (diffuse + specular);
		if (dot(N,L) <=0){
			light = ambiant;
		}
		return light;	
	}

	void main() { 
		vec3 N = normalize(texture(normal0, texCoord).xyz * 2.0f - 1.0f);

		float total_light = calcDirLight(N);

		//total_light  +=  Emitted;

		if (lampsActivated){
			for (int i = 1 ; i < n_lights ; i++){
				total_light += calcPointLight(i,N);
//				total_light += calcSpotLight(i,N);
			}
		}

		FragColor = vec4(vec3(texture(tex0, texCoord)) * total_light, 1.0); 
	}


