#version 330 core
    layout(location = 0) in vec3 vertex_position;
    layout(location = 4) in vec4 position;
    uniform mat4 V;
    uniform mat4 P;
    uniform float particleSize;
    out vec4 color_frag;
    out float lifetime;

    void main()
    {
        vec4 position_viewspace = V * vec4(position.xyz, 1);
        position_viewspace.xy += particleSize * (vertex_position.xy - vec2(0.5));
        gl_Position = P * position_viewspace;
         if (position.x > 3.0){
            color_frag = vec4(1.0, 1.0, 1.0, 0.0);
        }
        if (position.x < 2.0){
            color_frag = vec4(1.0, position.x/1.5, 0.0, 1.0 - position.y);
        }
        if (position.x <= 0.0){
            color_frag = vec4(1.0, -position.x/1.5, 0.0, 1.0 - position.y);
        }
        lifetime = position.w;
    }