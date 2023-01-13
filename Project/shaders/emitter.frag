#version 330 core
    in float lifetime;
    in vec4 color_frag;
    out vec4 fragColor;

    void main()
    {
        fragColor = color_frag;
    }