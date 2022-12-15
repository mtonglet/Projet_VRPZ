#version 330 core
	out vec4 FragColor;
	precision mediump float;
	in vec2 v_tex; 
	uniform sampler2D tex0; 
	void main()
	{ 
		FragColor = texture(tex0, v_tex); 
	}