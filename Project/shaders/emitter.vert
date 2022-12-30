#version 330 core
    layout(location = 0) in vec3 vertex_position;
    layout(location = 4) in vec4 position;
    uniform mat4 M_v;
    uniform mat4 M_p;
    uniform float particleSize;
    out float lifetime;

    void main()
    {
        vec4 position_viewspace = M_v * vec4(position.xyz, 1);
        position_viewspace.xy += particleSize * (vertex_position.xy - vec(0.5));
        gl_position = M_p * position_viewspace;
        lifetime = position.w;
    };