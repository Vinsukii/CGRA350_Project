#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform vec3 uColor;
uniform vec3 uScalar;

// viewspace data (this must match the output of the fragment shader)
in VertexData {
    vec3 position;
    vec3 normal;
    vec2 textureCoord;
    mat3 TBN;
} f_in;

// framebuffer output
out vec4 fb_color;

uniform sampler2D tex_0;
uniform sampler2D tex_1;
uniform sampler2D tex_2;
uniform sampler2D tex_3;


void main() {
    vec3 baseColor    = vec3(0.43, 0.28, 0.24); // flat color to help with normal map visibility
    vec3 burningColor = vec3(texture(tex_1, f_in.textureCoord)); // burning texture
    vec3 charredColor = vec3(texture(tex_2, f_in.textureCoord)); // charred texture
    vec3 normal = normalize(f_in.TBN * (texture(tex_3, f_in.textureCoord).rgb * 2.0 - 1)); // normal map via TBN matrix

    // calculate lighting (hack)
    vec3 eye = normalize(-f_in.position);
    float light = abs(dot(normal, eye));

    // texture blending during combustion via a vec3 scalar
    vec4 color = vec4(baseColor*uScalar[0] + burningColor*uScalar[1] + charredColor*uScalar[2], 1);


    // output to the frambuffer
    fb_color = mix(color/4, color, light*1.2);
}