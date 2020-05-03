#version 450
#extension GL_ARB_separate_shader_objects : enable

layout( binding = 1 ) uniform Ambient
{
	vec3 color;
	float strength;
} ambient;

layout( binding = 2 ) uniform Light
{
	vec3 position;
	vec3 color;
} light;

layout( binding = 3 ) uniform Material
{
	vec4 color;
	float metallic;
	float roughness;
	float ambient_occlusion;
} material;

layout( location = 0 ) in vec3 in_position;
layout( location = 1 ) in vec3 in_normal;
layout( location = 2 ) in vec4 in_color;

layout( location = 0 ) out vec4 out_color;


void main()
{
	vec3 ambient_color = ambient.strength * ambient.color;
	vec3 normal = normalize( in_normal );
	vec3 light_direction = normalize( light.position - in_position );
	float diffuse_factor = max( dot( normal, light_direction ), 0.0 );
	vec3 diffuse_color = diffuse_factor * light.color;
	vec4 light_color = vec4( ambient_color + diffuse_color, 1.0 );
	out_color = light_color * material.color;
}
