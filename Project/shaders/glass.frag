#version 330 core
	in vec2 TexCoords;

	out vec4 FragColor;
	precision mediump float;
	
	uniform sampler2D screenTexture;

	void main()
	{ 
		FragColor = vec4(vec3(texture(screenTexture, TexCoords)),1.0);
		//FragColor = vec4(1.0,0.0,0.0,1.0);
	}
	