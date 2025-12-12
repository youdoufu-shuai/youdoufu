#version 300 es
precision highp float;
out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture_diffuse1;

void main()
{
	FragColor = texture(texture_diffuse1, TexCoord);
	if (FragColor.a < 0.1) {
		discard;
	}
}