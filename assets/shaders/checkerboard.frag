#version 330 core

out vec4 frag_color;

// In this shader, we want to draw a checkboard where the size of each tile is (size x size).
// The color of the top-left most tile should be "colors[0]" and the 2 tiles adjacent to it
// should have the color "colors[1]".

//TODO: (Req 1) Finish this shader.

uniform int size = 32;
uniform vec3 colors[2];

void main(){
    
    // the position of the pixel in the checkboard
    vec2 pos = floor(gl_FragCoord.xy / size); //gl_FragCoord: contains the window-relative coordinates of the current fragment
    
    // if the sum of the x and y coordinates of the pixel is even, then the color is colors[0]
    // otherwise, the color is colors[1]
    if(mod(pos.x+pos.y,2.0) == 0.0){
        frag_color=vec4(colors[0],1.0);
    }else{
        frag_color=vec4(colors[1],1.0);
    }
}