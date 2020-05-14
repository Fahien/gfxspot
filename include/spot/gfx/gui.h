#pragma once

#include "spot/gfx/images.h"
#include "spot/gfx/shader.h"
#include "spot/gfx/pipelines.h"
#include "spot/gfx/descriptors.h"

#include "spot/math/math.h"

namespace spot::gfx
{


struct Constants
{
	math::Mat4 transform = math::Mat4::identity;
};


class CommandBuffer;

class Gui
{
  public:
	Gui( Device& d, const VkExtent2D& extent );
	~Gui();

	void update( const float delta_time );

	void draw( CommandBuffer& cb );

	Images images;

	ShaderModule vert;
	ShaderModule frag;
	PipelineLayout layout;

	Sampler sampler;
	Handle<ImageView> font_view;
};

class GraphicsPipeline;


}  // namespace spot::gfx