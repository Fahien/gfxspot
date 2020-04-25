#pragma once

#include <spot/gltf/handle.h>

namespace spot::gfx
{

class Gltf;

class Animations
{
  public:
	static void update( float dt, const Handle<Gltf>& model );
};


} // namespace gfx::spot
