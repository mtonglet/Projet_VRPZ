#version 330 core
#define MAX_LIGHTS_NUMBER 10
	out vec4 FragColor;
	precision mediump float;


	in vec3 Normal;
	in vec2 texCoord; 
	in vec3 fragCoord;
	in vec3 u_view_pos;
	in vec3 lights[10];
	uniform int n_lights;

	uniform sampler2D tex0; 
	uniform sampler2D normal0;

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
	uniform LightParams light_param;
	uniform bool lampsActivated;
	
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

	float calcDirLight(vec3 N){
		vec3 L = normalize(lights[0]); 
		vec3 V = normalize(u_view_pos - fragCoord); 
		float specular = specularCalculation( N, L, V, 0); 
		float diffuse = light_param.diffuse_strength * max(dot(N,L),0.0);
		float light = light_param.ambient_strength + diffuse + specular;
		if (dot(N,L) <= 0){
			light = light_param.ambient_strength + diffuse + specular; //
		}
		return light;
	}//ok

	float calcLight(int i, vec3 N){
		vec3 L = normalize(lights[i] - fragCoord) ; 
		vec3 V = normalize(u_view_pos - fragCoord); 
		float specular = specularCalculation( N, L, V, i); 
		float diffuse = light_param.diffuse_strength * max(dot(N,L),0.0);
		float distance = length(lights[i] - fragCoord) + length(u_view_pos - fragCoord);
		float attenuation = 1 / (light_param.constant + light_param.linear * distance + light_param.quadratic * distance * distance);
		float light =  attenuation * (diffuse + specular);
		if (dot(N,L) <=0){
			light = 0.0;
		}
		return light;
	}

	void main() { 
		vec3 N = normalize(texture(normal0, texCoord).xyz * 2.0f - 1.0f);

		float total_light = calcDirLight(N);

		//total_light  +=  Emitted;

		if (lampsActivated){
			for (int i = 1 ; i < n_lights ; i++){
				total_light += calcLight(i,N);
			}
		}

		FragColor = vec4(vec3(texture(tex0, texCoord)) * total_light, 1.0); 
	}


