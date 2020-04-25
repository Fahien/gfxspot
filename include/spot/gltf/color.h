#pragma once

namespace spot::gfx
{


struct Color
{
	static const Color black;
	static const Color white;
	static const Color red;
	static const Color green;
	static const Color blue;
	static const Color yellow;

	Color( float rr = 0.0f, float gg = 0.0f, float bb = 0.0f, float aa = 1.0f )
	: r { rr }, g { gg }, b { bb }, a { aa } {}

	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 1.0f;
};



} // namespace spot::gfx
