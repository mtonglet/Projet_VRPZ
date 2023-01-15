#version 400 core
	out vec4 FragColor;
	precision mediump float; 

	
	in vec3 pass_normal;
	in vec3 reflectedVector;
	

	uniform samplerCube cubemapSampler;
	uniform vec3 color;
	

	const vec3 lightDirection = normalize(vec3(0.0,3.0, -3.0));
	const float ambient = 0.3;
	
	void main(void){
	
	float brightness = 0.5*max(dot(-lightDirection, normalize(pass_normal)), 0.0) + ambient;
	vec4 out_Color = vec4(color*brightness, 1.0f);
	
	vec4 reflectedColour = texture(cubemapSampler, reflectedVector);
	
	FragColor = vec4(mix(out_Color, reflectedColour, 0.5));

}