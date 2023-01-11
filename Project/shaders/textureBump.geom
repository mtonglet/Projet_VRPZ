#version 330 core 

#define MAX_LIGHTS_NUMBER 10

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 Normal;
out vec2 texCoord;
out vec3 fragCoord;
out vec3 u_view_pos;
out vec3 lights[10];
//out int n_lights;
out vec4 u_frag_pos_light;

in DATA
{
    vec3 Normal;
    vec2 texCoord;
    vec3 fragCoord;
    mat4 projection;
    mat4 model;
    vec3 u_view_pos;
    vec3 lights[10];
//    int n_lights;
	vec4 u_frag_pos_light;
} data_in[];

uniform int n_lights;

void main() 
{
    //formula to get the normal map to work correctly (TBN)
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec2 deltaUV0 = data_in[1].texCoord - data_in[0].texCoord;
    vec2 deltaUV1 = data_in[2].texCoord - data_in[0].texCoord;

    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

    vec3 T = normalize(vec3(data_in[0].model * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(data_in[0].model * vec4(bitangent, 0.0f)));
    vec3 N = normalize(vec3(data_in[0].model * vec4(cross(edge1, edge0), 0.0f)));

    mat3 TBN = mat3(T,B,N);
    TBN = transpose(TBN);

    gl_Position = data_in[0].projection * gl_in[0].gl_Position;
//    n_lights = data_in[0].n_lights;
    Normal = data_in[0].Normal;
    texCoord = data_in[0].texCoord;
    u_frag_pos_light = data_in[0].u_frag_pos_light;
    fragCoord = TBN * data_in[0].fragCoord;
    for (int i = 0; i<n_lights;i++){
        lights[i] = TBN * data_in[0].lights[i];
    }
    u_view_pos = TBN * data_in[0].u_view_pos;

    EmitVertex();

    gl_Position = data_in[1].projection * gl_in[1].gl_Position;
    Normal = data_in[1].Normal;
    texCoord = data_in[1].texCoord;
    u_frag_pos_light = data_in[1].u_frag_pos_light;
    fragCoord = TBN * data_in[1].fragCoord;
    //lightPos = TBN * data_in[1].lightPos;
    for (int i = 0; i<n_lights;i++){
        lights[i] = TBN * data_in[1].lights[i];
    }
    u_view_pos = TBN * data_in[1].u_view_pos;
    EmitVertex();

    gl_Position = data_in[2].projection * gl_in[2].gl_Position;
    Normal = data_in[2].Normal;
    texCoord = data_in[2].texCoord;
    fragCoord = TBN * data_in[2].fragCoord;
    u_frag_pos_light = data_in[2].u_frag_pos_light;
    //lightPos = TBN * data_in[2].lightPos;
    for (int i = 0; i<n_lights;i++){
        lights[i] = TBN * data_in[2].lights[i];
    }    
    u_view_pos = TBN * data_in[2].u_view_pos;
    EmitVertex();
    EndPrimitive();

}