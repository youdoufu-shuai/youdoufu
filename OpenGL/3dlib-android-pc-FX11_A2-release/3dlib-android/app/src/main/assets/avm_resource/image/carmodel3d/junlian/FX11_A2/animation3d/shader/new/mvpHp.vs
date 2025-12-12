#version 300 es
precision highp float;
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;

out vec2 vecTexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    vecTexCoord = a_texCoord;    
    gl_Position = u_Projection * u_View * u_Model * vec4(a_position, 1.0);
}