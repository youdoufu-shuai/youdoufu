#version 300 es
precision mediump float;
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;

out vec2 vecTexCoord;

uniform mat4 u_MVP;

void main()
{
    vecTexCoord = a_texCoord;    
    gl_Position = u_MVP * vec4(a_position, 1.0f);
}