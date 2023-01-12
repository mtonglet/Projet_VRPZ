#version 330 core	
	precision mediump float;

	in vec4 v_frag_coord;

	uniform float far_plane;
	uniform vec3 light_pos;

	void main() { 
		gl_FragDepth = length(v_frag_coord.xyz - light_pos) / far_plane;
	}