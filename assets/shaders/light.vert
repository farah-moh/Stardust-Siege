#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;

uniform mat4 M;
uniform mat4 M_I_T;
uniform mat4 VP;
uniform vec3 camera_position;

out Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 world;
    vec3 view;
    vec3 normal;
} vsout;

void main() {
    vsout.world = (M * vec4(position, 1.0f)).xyz;
    vsout.view = camera_position - vsout.world;
    vsout.normal = normalize((M_I_T * vec4(normal, 0.0f)).xyz);
    gl_Position = VP * vec4(vsout.world, 1.0);
    vsout.color = color;
    vsout.tex_coord = tex_coord;
}