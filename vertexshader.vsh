#version 130

in vec4 vPosition;
//in vec4 vColor;
in vec2 vTextureCoordinate;

uniform mat4 modelView;
uniform mat4 projection;

//out vec4 color;
out vec2 fTextureCoordinate;

void main(void)
{
    gl_Position = projection * modelView * vPosition;
    fTextureCoordinate = vTextureCoordinate;
}

