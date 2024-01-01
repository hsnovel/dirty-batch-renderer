#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 proj;
uniform mat4 model;
uniform mat4 view;

void main() {
	// gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	gl_Position = proj * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
