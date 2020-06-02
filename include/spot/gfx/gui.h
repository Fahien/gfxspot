#pragma once

#include "spot/gfx/images.h"
#include "spot/gfx/shader.h"
#include "spot/gfx/pipelines.h"
#include "spot/gfx/descriptors.h"

#include "spot/math/math.h"
#include "spot/math/mat4.h"

namespace spot::gfx
{


struct Constants
{
	math::Mat4 transform = math::Mat4::Identity;
};


class CommandBuffer;
class Window;

class Gui
{
  public:
	Gui( Device& d, Window& window, VkExtent2D extent );
	~Gui();

	void update( float delta_time );

	void draw( CommandBuffer& cb );

	Window& window;
	Images images;

	ShaderModule vert;
	ShaderModule frag;
	PipelineLayout layout;

	Sampler sampler;
	Handle<ImageView> font_view;
};

class GraphicsPipeline;


}  // namespace spot::gfx