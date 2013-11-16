#version 120

uniform vec2 position = vec2(0.0);
uniform float zoom = 1.0;

varying vec2 coord;

void main() {
	vec2 v_pos = gl_Vertex.xy;
	gl_Position = vec4(v_pos, 0.0, 1.0);
	coord = position + v_pos * zoom;
}
