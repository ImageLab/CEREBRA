#version 430

in vec4 vColor;
in vec4 vPosition;
in vec2 vTextureCoordinate;

uniform mat4 modelView;
uniform mat4 projection;

out vec2 fTextureCoordinate;
out vec4 color;

void main(void)
{
    gl_Position = projection * modelView * vPosition;
    fTextureCoordinate = vTextureCoordinate;
    color = vColor;
}

