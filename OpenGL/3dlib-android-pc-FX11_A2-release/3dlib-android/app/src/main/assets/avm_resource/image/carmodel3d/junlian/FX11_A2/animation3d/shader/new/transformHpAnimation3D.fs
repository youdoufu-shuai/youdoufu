#version 300 es
precision highp float;
out vec4 FragColor;

in vec2 vecTexCoord;

uniform sampler2D texture_diffuse1;

uniform float f_alpha;

void main()
{    
    FragColor   = texture(texture_diffuse1, vecTexCoord);
    FragColor.a = f_alpha;
}