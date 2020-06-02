#version 450

layout ( binding = 0 ) uniform sampler2D color;

layout ( location = 0 ) in vec2 in_uv;

layout ( location = 0 ) out vec4 out_color;

void main()
{
	out_color = texture( color, in_uv );
}
