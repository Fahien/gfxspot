#pragma once

#include <spot/gltf/handle.h>

namespace spot::gfx
{

class Gltf;

class Animations
{
  public:
	void update( float dt, const Handle<Gltf>& model );

	bool pause = false;
};


} // namespace gfx::spot
