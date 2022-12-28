#version 330 core
	out vec4 FragColor;
	precision mediump float;
	
	uniform vec3 colorRGB;
	uniform float alpha;

	void main()
	{ 
		FragColor = vec4(colorRGB,alpha);
		//FragColor = vec4(0.5,0.4,0.5,1.0);
		//FragColor = vec4(vec3(red,green,blue), 1.0);
	}