#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform vec3 uColor;
uniform vec2 uWindowSize;

// mesh data
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aPosOffset;
layout(location = 2) in vec3 aColor;
layout(location = 3) in float aAlpha;

// model data (this must match the input of the vertex shader)
out VertexData {
	vec3 position;
	vec3 color;
	float alpha;
} v_out;

void main() {

	// transform vertex data to viewspace
	v_out.position = (uModelViewMatrix * vec4(aPosition+aPosOffset, 1)).xyz;
	v_out.color = aColor;
	v_out.alpha = aAlpha;
	//v_out.normal = normalize((uModelViewMatrix * vec4(aNormal, 0)).xyz);
	//v_out.textureCoord = aTexCoord;

	// set the screenspace position (needed for converting to fragment data)
	gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition+aPosOffset, 1);
	if (aColor.z != 0) {
		gl_PointSize = 40 * uWindowSize.y * 0.018 / gl_Position.w;
	} else {
		gl_PointSize = (1-abs(aAlpha)) * 60 * uWindowSize.y * 0.018 / gl_Position.w;
	}




}