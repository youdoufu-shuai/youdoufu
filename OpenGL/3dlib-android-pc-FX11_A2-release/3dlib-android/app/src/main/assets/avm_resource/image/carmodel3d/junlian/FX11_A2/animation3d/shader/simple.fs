#version 300 es
// precision highp float;
precision mediump float;

in  vec2 TexCoord;
out vec4 FragColor;

// texture sampler
uniform sampler2D texture_diffuse1;

void main()
{
	FragColor = texture(texture_diffuse1, TexCoord);
	// if (FragColor.a < 0.1) {
	// 	discard;
	// }
}
