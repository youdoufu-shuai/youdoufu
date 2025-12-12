#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 vecTexCoord;

uniform sampler2D textureDiffuse1;

void main()
{    
    FragColor = vec4(1.f,1.f,1.f, 1.0f);///texture(textureDiffuse1, vecTexCoord);
}