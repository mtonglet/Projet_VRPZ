//from Tutorial 26 - Victor Gordan
#version 330 core 

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 VPshadows[6];

out vec4 v_frag_coord;


void main() 
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face;
        for(int j = 0; j < 3; ++j)
        {
            v_frag_coord = gl_in[j].gl_Position;
            gl_Position = VPshadows[face] * v_frag_coord;
            EmitVertex();
        }
        EndPrimitive();
    }

}