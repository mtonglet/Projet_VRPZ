#version 330 core
	in vec3 position; 
	in vec2 tex_coord; 
	in vec3 normal; 

	out vec3 pass_normal;
	out vec3 reflectedVector;
	

	uniform mat4 M;
	uniform mat4 P;
	uniform mat4 V;
	uniform mat4 itM;
	uniform vec3 u_view_pos;

	void main(void){

		vec4 worldPosition = M * vec4(position, 1.0);
		gl_Position = P * V* worldPosition;
	
		//pass_normal = (itM * vec4(normal,1.0)).xyz;
		pass_normal = normalize((itM * vec4(normal,1.0)).xyz);
		vec3 unitNormal = normalize((itM * vec4(normal,1.0)).xyz);
		
		vec3 viewVector = normalize(worldPosition.xyz - u_view_pos);
		reflectedVector = reflect(viewVector, unitNormal);

		if(tex_coord.x>5.0f){
		reflectedVector = vec3(1.0);
		}


}