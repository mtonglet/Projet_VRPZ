#version 330 core
	#define MAX_LIGHTS_NUMBER 10

	in vec3 position; 
	in vec2 tex_coord; 
	in vec3 normal;

	out DATA
	{
		vec3 Normal;
		vec2 texCoord;
		vec3 fragCoord;
		mat4 projection;
		mat4 model;
		vec3 u_view_pos;
		vec3 lights[10];
	} data_out;


	uniform vec3 u_view_pos;
	uniform vec3 lights[10];
	uniform mat4 M; 
	uniform mat4 R;
	uniform mat4 V; 
	uniform mat4 P; 
	uniform mat4 itM;

	void main()
	{
		gl_Position = R * M * vec4(position, 1.0f);
		data_out.Normal = vec3(itM * vec4(normal, 1.0));
		data_out.texCoord = tex_coord;
		data_out.fragCoord = gl_Position.xyz;
		data_out.projection = P * V;
		data_out.model = M;
		data_out.lights = lights;
		data_out.u_view_pos = u_view_pos;
	}