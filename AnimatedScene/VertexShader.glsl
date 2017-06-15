#version 330

in vec4 position;
in vec3 normal;
in vec2 texel;

out vec3 vertex_position;
out vec3 vertex_normal_vec;
out vec2 vertex_texel;

uniform mat4 pvm_matrix;
uniform mat3 normal_matrix;
uniform mat4 model_matrix; // for vertex position in world space

void main()
{
	vertex_position = (model_matrix * position).xyz;
	vertex_normal_vec = normalize(normal_matrix * normal);
	vertex_texel = texel;
	gl_Position = pvm_matrix * position;
}
