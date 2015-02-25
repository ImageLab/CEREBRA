#version 130

in vec4 vPosition;
in vec4 vColor;

uniform mat4 modelView;
uniform mat4 projection;

out vec4 color;

void main(void)
{
    gl_Position = projection * modelView * vPosition;
    color = vColor;
}

