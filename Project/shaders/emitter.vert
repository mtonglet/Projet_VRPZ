#version 330 core
    layout(location = 0) in vec3 vertex_position;
    layout(location = 4) in vec4 position;
    uniform mat4 V;
    uniform mat4 P;
    uniform float particleSize;
    out float lifetime;

    void main()
    {
        vec4 position_viewspace = V * vec4(position.xyz, 1);
        position_viewspace.xy += particleSize * (vertex_position.xy - vec2(0.5));
        gl_Position = P * position_viewspace;
        lifetime = position.w;
    }