#pragma once


namespace spot::gfx
{

class Model;

class Animations
{
  public:
	void update( float dt, const Model& model );

	bool pause = false;
};


} // namespace gfx::spot
