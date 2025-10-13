#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D texture_outter;
uniform sampler2D texture_inner;

void main()
{             
    FragColor = texture(texture_outter, TexCoords);

    // set contrast special effect
    /*
    if (gl_FragCoord.x < 400) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    } else {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
    */

    // set two-side(inner face and outter face) different texture
    if (gl_FrontFacing) {
        FragColor = texture(texture_outter, TexCoords);
    } else {
        FragColor = texture(texture_inner, TexCoords);
    }
}