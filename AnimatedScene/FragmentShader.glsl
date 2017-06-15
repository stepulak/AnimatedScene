#version 330

out vec4 final_color;

in vec3 vertex_position;
in vec3 vertex_normal_vec;
in vec2 vertex_texel;

uniform vec3 eye_position;
uniform sampler2D texture_sampler;

// fragment material
uniform vec3 material_ambient_color;
uniform vec3 material_diffuse_color;
uniform vec3 material_specular_color;
uniform float material_shininess;

uniform int texture_type;

// lights
#define MAX_LIGHTS 5

uniform int num_point_lights;

struct point_light_data {
	vec4 position;
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
};

layout(std140) uniform point_lights_data {
	point_light_data point_lights[MAX_LIGHTS];
};

uniform int num_spot_lights;

struct spot_light_data {
	vec4 position;
	vec3 direction;
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
	float angle;
};

layout(std140) uniform spot_lights_data {
	spot_light_data spot_lights[MAX_LIGHTS];
};

// Normal light = point light or directional light
void compute_normal_light(out vec3 light_color,
	in vec4 light_position, 
	in vec3 light_ambient_color,
	in vec3 light_diffuse_color,
	in vec3 light_specular_color)
{
	vec3 light_vec = normalize(light_position.xyz - light_position.w * vertex_position);
	vec3 eye_vec = normalize(eye_position - vertex_position);
	vec3 mid_eye_light_vec = normalize(eye_vec + light_vec);
	
	// Shorten the distance of the vertex to the light source
	float dist = distance(light_position.xyz, vertex_position) * light_position.w / 2.5;
	float penetration = 1.0 / (1.0 + 0.1 * dist + 0.01 * dist * dist);
	
	// lambert model
	float diffuse_intensity = max(dot(light_vec, vertex_normal_vec), 0.0) * penetration;

	// blinn-phong model
	float cosine_angle = max(dot(mid_eye_light_vec, vertex_normal_vec), 0.0);
	float specular_intensity = pow(cosine_angle, material_shininess) * diffuse_intensity;

	vec3 ambient_color = light_ambient_color * material_ambient_color;
	vec3 diffuse_color =  light_diffuse_color * material_diffuse_color * diffuse_intensity;
	vec3 specular_color = light_specular_color * material_specular_color * specular_intensity;

	// return
	light_color = ambient_color + diffuse_color + specular_color;
}

void compute_spot_light(out vec3 light_color,
	in vec4 light_position,
	in vec3 light_direction,
	in float light_angle,
	in vec3 light_ambient_color,
	in vec3 light_diffuse_color,
	in vec3 light_specular_color)
{
	vec3 light_vec = normalize(light_position.xyz - vertex_position);
	float angle = dot(light_vec, normalize(-light_direction));

	if (acos(angle) < light_angle) {
		// Inside spotlight? Then proceed as a normal light...
		compute_normal_light(light_color, light_position,
			light_ambient_color, light_diffuse_color, light_specular_color);
	} else {
		light_color = vec3(0.0, 0.0, 0.0);
	}
}

void compute_texel(out vec3 color)
{
	color = texture(texture_sampler, vertex_texel).rgb;
}

// Compute procedural wood with simple tree-rings
void compute_wood(out vec3 color)
{
	vec2 center = vec2(0.5, 0.5);
	float angle = dot(vertex_texel, center);
	float dist = distance(vertex_texel, center) + sin(3.0 * angle + 3.0 * vertex_texel.t);
	float skip = mod(10.0 * fract(dist), 1.01);
	float step_diff = smoothstep(0.1, 0.2, skip) - smoothstep(0.8, 0.9, skip);

	vec3 light_brown = vec3(0.54, 0.27, 0.07);
	vec3 dark_brown = vec3(0.39, 0.26, 0.12);

	color = mix(dark_brown, light_brown, step_diff);
}

// Compute procedural brick wall with next-row offset and with different brick colors
void compute_bricks(out vec3 color)
{
	vec2 repeat = vec2(30.0, 20.0);
	vec2 new_coord = repeat * vertex_texel;
	float offset = 0.3 * floor(mod(new_coord.t, 3.0));
	vec2 shifted_coord = fract(vec2(new_coord.s + offset, new_coord.t));
	vec2 step_diff = smoothstep(0.05, 0.1, shifted_coord) - smoothstep(0.9, 0.95, shifted_coord);
	
	vec3 dark_red = vec3(0.38, 0.0, 0.0);
	vec3 biege = vec3(0.68, 0.53, 0.46);
	vec3 dark_white = vec3(0.95, 0.95, 0.95);
	
	int mix_x = int(floor(mod(new_coord.t, 2.0)));
	int mix_y = int(step(floor(mod(new_coord.s + offset, 3.0)), 1.0));

	vec3 brick_color = mix(biege, dark_red, float(mix_x ^ mix_y));

	color = mix(dark_white, brick_color, step_diff.s * step_diff.t);
}

// Compute procedural carpet with little silver stars on the edges and red heart in the middle
void compute_carpet(out vec3 color)
{
	vec2 repeat = vec2(20.0, 20.0);
	vec2 coord = fract(repeat * vertex_texel);
	
	vec3 silver = vec3(0.75, 0.75, 0.75);
	vec3 light_blue = vec3(0.2, 0.2, 1.0);
	vec3 light_red = vec3(0.95, 0.35, 0.35);

	color = mix(light_blue, silver, smoothstep(0.55, 0.60, distance(coord, vec2(0.5, 0.5))));

	// heart: x^2 + (y - 3/4 * (x^2/3))^2 < 1
	vec2 centered = vec2(abs(coord.x - 0.5), coord.y - 0.33);
	float heart = centered.x * centered.x + pow((centered.y - 0.75 * pow(centered.x, 0.66)), 2);
	color = mix(color, light_red, smoothstep(0.15, 0.1, heart));
}

void main()
{
	vec3 total_light = vec3(0.0, 0.0, 0.0);
	
	for (int i = 0; i < num_point_lights; i++) {
		vec3 light = vec3(0.0, 0.0, 0.0);

		compute_normal_light(light, 
			point_lights[i].position,
			point_lights[i].ambient_color,
			point_lights[i].diffuse_color,
			point_lights[i].specular_color);

		total_light = clamp(total_light + light, 0.0, 1.0);
	}
	
	for (int i = 0; i < num_spot_lights; i++) {
		vec3 light = vec3(0.0, 0.0, 0.0);

		compute_spot_light(light,
			spot_lights[i].position,
			spot_lights[i].direction,
			spot_lights[i].angle,
			spot_lights[i].ambient_color,
			spot_lights[i].diffuse_color,
			spot_lights[i].specular_color);

		total_light = clamp(total_light + light, 0.0, 1.0);
	}

	vec3 color;

	if (texture_type == 1) {
		compute_texel(color);
	} else if (texture_type == 2) {
		compute_wood(color);
	} else if (texture_type == 3) {
		compute_bricks(color);
	} else if (texture_type == 4) {
		compute_carpet(color);
	} else {
		color = vec3(1.0, 1.0, 1.0);
	}

	final_color = vec4((clamp(total_light, 0.0, 1.0) * color).xyz, 1.0);
	
	// Apply fog
	vec4 fog_color = vec4(0.5, 0.5, 0.5, 1.0);
	float fog_density = 0.05;

	// distance = distance of vertex from the center [0, 0, 0]
	float exp_factor = 1.0 / exp(length(vertex_position) * fog_density);

	final_color = mix(fog_color, final_color, clamp(exp_factor, 0.0, 1.0));
}