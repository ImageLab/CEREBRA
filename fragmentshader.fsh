#version 130

//in vec4 color;
uniform sampler2D texture;
uniform vec2 textureOffset;

in vec2 fTextureCoordinate;

out vec4 fColor;
void main(void)
{
    //fColor = color;
    fColor = texture2D(texture, fTextureCoordinate + textureOffset);
}

