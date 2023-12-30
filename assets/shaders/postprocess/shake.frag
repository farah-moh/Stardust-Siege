#version 330
uniform sampler2D tex;
uniform float time;  // Uniform variable for time
uniform float speed=5;
uniform float amplitude=0.5;
uniform float frequency=3;
in vec2 tex_coord;
out vec4 frag_color;

void main() {
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = tex_coord;

    // Create a shaking effect by slightly offsetting the texture coordinates
    vec2 dt = vec2(0.0, 0.0);
    dt.x = 0.01 * sin(time * 100.0 + 1.0);
    dt.y = 0.01 * cos(time * 100.0 + 1.0);

    // Sample the texture at the uv + dt coordinates
    frag_color = texture(tex, uv + dt);
}