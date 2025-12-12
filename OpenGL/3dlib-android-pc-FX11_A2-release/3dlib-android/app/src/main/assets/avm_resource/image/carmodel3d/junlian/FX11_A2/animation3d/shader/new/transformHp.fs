#version 300 es
precision highp float;
out vec4 FragColor;

in vec2 vecTexCoord;

uniform sampler2D texture_diffuse1;
uniform float offset;

void main()
{
    // FragColor = vec4(0.3 + offset, 1.0, 0.5, 1.0);
    // FragColor = texture(texture_diffuse1, vecTexCoord + vec2(offset, offset));
    FragColor = texture(texture_diffuse1, vecTexCoord);

}