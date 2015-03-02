#version 130

uniform sampler2D texture;
uniform vec2 textureOffset;

in vec2 fTextureCoordinate;

out vec4 fColor;
void main(void)
{
    fColor = texture2D(texture, fTextureCoordinate + textureOffset);
}

