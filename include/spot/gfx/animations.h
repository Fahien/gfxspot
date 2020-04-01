#pragma once

namespace spot
{

namespace gltf
{
class Gltf;
} // namespace gltf

namespace gfx
{


class Animations
{
  public:
	static void update( float dt, gltf::Gltf& model );

  private:
};


} // namespace gfx
} // namespace spot
