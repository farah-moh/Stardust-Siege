#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
// Now we need to the surface normal to compute the light so we will send it as an attribute.
layout(location = 3) in vec3 normal;

// We will need to do the light processing in the world space so we will break our transformations into 2 stages:
// 1-object world
uniform mat4 M; //matrix that transforms from local space to world space
uniform mat4 M_I_T; // matrix for transforming normal vectors to world space using inverse transform matrix
// // 2- World to Homogenous Clipspace.
uniform mat4 VP; // matrix for transforming from world space to homogenous clipping space
uniform vec3 camera_position; // camera position which will be used in specular calculation

//our varyings to be sent to frag shader:
out Varyings {
    vec4 color; //color of vertix
    vec2 tex_coord; //texture coordinates of vertix
    vec3 world; //vertix position in world space
    vec3 view; //vector from vertix to camera
    vec3 normal; //surface normal in world space
} vsout;

void main() {
    vsout.world = (M * vec4(position, 1.0f)).xyz;  //  compute the world position (1.0f as position is a point not a vector).
    vsout.view = camera_position - vsout.world;  // //compute the view vector (vertex to eye vector in the world space) to be used for specular computation later.
    vsout.normal = normalize((M_I_T * vec4(normal, 0.0f)).xyz); // compute normal in the world space (Note that w=0 since this is a vector).
    // Finally, we compute the position in the homogenous clip space and send the rest of the data
    gl_Position = VP * vec4(vsout.world, 1.0);
    vsout.color = color;
    vsout.tex_coord = tex_coord;
}