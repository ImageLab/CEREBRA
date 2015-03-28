#version 430

uniform sampler2D texture;
uniform vec2 textureOffset;
uniform bool isTextured;

in vec2 fTextureCoordinate;
in vec4 color;

out vec4 fColor;
void main(void)
{
    if( isTextured)
        fColor = texture2D(texture, fTextureCoordinate + textureOffset);
    else
        fColor = color;
}

