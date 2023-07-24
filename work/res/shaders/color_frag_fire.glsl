#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform sampler2D uTextureFire;
uniform vec2 uWindowSize;

// viewspace data (this must match the output of the fragment shader)
in VertexData {
	vec3 position;
	vec3 color;
	float alpha;
} f_in;

// framebuffer output
out vec4 fb_color;

void main() {
	// calculate lighting (hack)
	//vec3 eye = normalize(-f_in.position);
	//float light = abs(dot(normalize(vec3(0,0,1)), eye));
	//vec4 texCol = (texture(uTexture, f_in.textureCoord) * vec4(uColor, 1));
	//vec4 color = mix(texCol / 4, texCol, light);

	// output to the frambuffer
	vec2 texCoord = (vec4(gl_PointCoord, 0, 1)).xy;
	vec4 texColor = texture(uTextureFire, gl_PointCoord);
	if (texColor.a < 0.1)
		discard;

	fb_color = vec4(vec3(texColor)*f_in.color, texColor.a*f_in.alpha);
}