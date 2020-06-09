#pragma once

#include <spot/math/math.h>

namespace spot::gfx
{


class Input
{
  public:
	math::Vec2 scroll = {};

	math::Vec2 swipe = {};

	math::Vec2 cursor = {};

	struct Press
	{
		bool left = false;
		bool middle = false;
		bool right = false;

	} press;
	
	struct Click
	{
		bool left = false;
		bool middle = false;
		bool right = false;

		/// Origin of click
		math::Vec2 pos = {};
	} click;

	struct Key
	{
		bool w = false;
		bool a = false;
		bool s = false;
		bool d = false;
		bool r = false;
		bool enter = false;

		void reset();
	};

	struct KeyState
	{
		/// True when the key is just pressed
		Key down;

		/// True when the key is still pressed
		Key value;

		/// True when the key is just released
		Key up;
	} key;
};


} // namespace spot::gfx
