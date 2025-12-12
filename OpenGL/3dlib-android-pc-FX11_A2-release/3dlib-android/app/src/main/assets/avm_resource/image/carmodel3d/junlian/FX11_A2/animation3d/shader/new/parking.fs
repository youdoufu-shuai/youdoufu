#version 300 es
precision mediump float;
out vec4 FragColor;
in vec2 vecTexCoord;
uniform sampler2D textureDiffuse1;

void main()
{   
    FragColor = texture(textureDiffuse1, vecTexCoord);
    // if (FragColor.a < 0.1f)
    //     discard;
}
