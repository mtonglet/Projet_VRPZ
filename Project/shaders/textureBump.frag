#version 330 core
	out vec4 FragColor;
	precision mediump float;

	in vec2 texCoord; 
	in vec3 fragCoord;
	in vec3 Normal;

	uniform vec3 u_view_pos;
	uniform sampler2D tex0; 
	uniform sampler2D normal0;

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

	uniform Light light;

	uniform float shininess; 
	


	float specularCalculation(vec3 N, vec3 L, vec3 V ){ 
		vec3 R = reflect (-L,N);  //reflect (-L,N) is  equivalent to //max (2 * dot(N,L) * N - L , 0.0) ;
		float cosTheta = dot(R , V); 
		float spec = pow(max(cosTheta,0.0), 32.0); 
		return light.specular_strength * spec;
	}

	//void main() { 
	//	vec3 N = normalize(Normal);
	//	vec3 L = normalize(light.light_pos - fragCoord) ; 
	//	vec3 V = normalize(u_view_pos - fragCoord); 
	//	float specular = specularCalculation( N, L, V); 
	//	float diffuse = light.diffuse_strength * max(dot(N,L),0.0);
	//	float distance = length(light.light_pos - fragCoord) + length(u_view_pos - fragCoord);
	//	float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	//	float light = light.ambient_strength + attenuation * (diffuse + specular); 
	//	FragColor = vec4(vec3(texture(tex0, texCoord)) * vec3(light), 1.0); 
	//}

	void main() { 
		vec3 N = normalize(texture(normal0, texCoord).xyz * 2.0f - 1.0f);
		vec3 L = normalize(light.light_pos - fragCoord) ; 
		vec3 V = normalize(u_view_pos - fragCoord); 
		float specular = specularCalculation( N, L, V); 
		float diffuse = light.diffuse_strength * max(dot(N,L),0.0);
		float distance = length(light.light_pos - fragCoord) + length(u_view_pos - fragCoord);
		float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
		float light = light.ambient_strength + attenuation * (diffuse + specular); 
		FragColor = vec4(vec3(texture(tex0, texCoord)) * vec3(light), 1.0); 
	}
