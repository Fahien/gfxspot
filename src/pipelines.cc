#include "spot/gfx/pipelines.h"

#include <cassert>

#include "spot/gfx/graphics.h"
#include "spot/gfx/hash.h"
#include "spot/gfx/renderer.h"

namespace spot::gfx
{
PipelineLayout::PipelineLayout(
	Device& d,
	const std::vector<VkDescriptorSetLayoutBinding>& bindings,
	std::optional<VkPushConstantRange> constants
)
    : device { d }
    , descriptor_set_layout { d, std::move( bindings ) }
{
	VkPipelineLayoutCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	info.setLayoutCount = 1;
	info.pSetLayouts = &descriptor_set_layout.handle;

	if ( constants )
	{
		info.pushConstantRangeCount = 1;
		info.pPushConstantRanges = &constants.value();
	}

	const auto res = vkCreatePipelineLayout( device.handle, &info, nullptr, &handle );
	assert( res == VK_SUCCESS && "Cannot create pipeline layout" );
}


PipelineLayout::~PipelineLayout()
{
	if ( handle != VK_NULL_HANDLE )
	{
		vkDestroyPipelineLayout( device.handle, handle, nullptr );
	}
}


GraphicsPipeline::GraphicsPipeline( const std::vector<VkVertexInputBindingDescription>& bindings,
                                    const std::vector<VkVertexInputAttributeDescription>& attributes,
									PipelineLayout& layo,
                                    ShaderModule& vert,
									ShaderModule& frag,
									RenderPass& render_pass,
                                    const VkViewport& viewport,
									const VkRect2D& scissor,
									const VkPipelineColorBlendAttachmentState& color_blend_attachment,
									std::vector<VkDynamicState> dynamic_states,
									VkCullModeFlags cull_mode,
									VkBool32 depth_test,
									const VkPrimitiveTopology topology )
    : device { vert.device }
    , layout { layo }
{
	VkPipelineVertexInputStateCreateInfo input_info = {};
	input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	input_info.vertexBindingDescriptionCount = bindings.size();
	input_info.pVertexBindingDescriptions = bindings.data();
	input_info.vertexAttributeDescriptionCount = attributes.size();
	input_info.pVertexAttributeDescriptions = attributes.data();

	VkPipelineInputAssemblyStateCreateInfo assembly_info = {};
	assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assembly_info.topology = topology;
	assembly_info.primitiveRestartEnable = VK_FALSE;

	std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages = {};

	auto& vert_info = shader_stages[0];
	vert_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_info.module = vert.handle;
	vert_info.pName = "main";

	auto& frag_info = shader_stages[1];
	frag_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_info.module = frag.handle;
	frag_info.pName = "main";

	VkPipelineViewportStateCreateInfo viewport_info = {};
	viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_info.viewportCount = 1;
	viewport_info.pViewports = &viewport;
	viewport_info.scissorCount = 1;
	viewport_info.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer_info = {};
	rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer_info.depthClampEnable = VK_FALSE;
	rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
	rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer_info.lineWidth = 1.0f;
	rasterizer_info.cullMode = cull_mode;
	rasterizer_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer_info.depthBiasEnable = VK_FALSE;
	rasterizer_info.depthBiasConstantFactor = 0.0f;
	rasterizer_info.depthBiasClamp = 0.0f;
	rasterizer_info.depthBiasSlopeFactor = 0.0f;

	VkPipelineDepthStencilStateCreateInfo depth_info = {};
	depth_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_info.depthTestEnable = depth_test;
	depth_info.depthWriteEnable = depth_test;
	depth_info.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_info.depthBoundsTestEnable = VK_FALSE;
	depth_info.stencilTestEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling_info = {};
	multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling_info.sampleShadingEnable = VK_FALSE;
	multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling_info.minSampleShading = 1.0f;           // Optional
	multisampling_info.pSampleMask = nullptr;             // Optional
	multisampling_info.alphaToCoverageEnable = VK_FALSE;  // Optional
	multisampling_info.alphaToOneEnable = VK_FALSE;       // Optional


	VkPipelineColorBlendStateCreateInfo color_blending = {};
	color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blending.logicOpEnable = VK_FALSE;
	color_blending.logicOp = VK_LOGIC_OP_COPY;  // Optional
	color_blending.attachmentCount = 1;
	color_blending.pAttachments = &color_blend_attachment;
	color_blending.blendConstants[0] = 0.0f;  // Optional
	color_blending.blendConstants[1] = 0.0f;  // Optional
	color_blending.blendConstants[2] = 0.0f;  // Optional
	color_blending.blendConstants[3] = 0.0f;  // Optional

	VkPipelineDynamicStateCreateInfo dynamic_state = {};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = uint32_t( dynamic_states.size() );
	dynamic_state.pDynamicStates = dynamic_states.size() ? dynamic_states.data() : nullptr;

	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = shader_stages.size();
	pipeline_info.pStages = shader_stages.data();

	pipeline_info.pVertexInputState = &input_info;
	pipeline_info.pInputAssemblyState = &assembly_info;
	pipeline_info.pViewportState = &viewport_info;
	pipeline_info.pRasterizationState = &rasterizer_info;
	pipeline_info.pMultisampleState = &multisampling_info;
	pipeline_info.pDepthStencilState = &depth_info;
	pipeline_info.pColorBlendState = &color_blending;
	pipeline_info.pDynamicState = &dynamic_state;

	pipeline_info.layout = layout.handle;

	pipeline_info.renderPass = render_pass.handle;
	pipeline_info.subpass = 0;

	auto ret = vkCreateGraphicsPipelines( device.handle, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &handle );
	assert( ret == VK_SUCCESS && "Cannote create graphics line_pipeline" );
}


GraphicsPipeline::~GraphicsPipeline()
{
	if ( handle != VK_NULL_HANDLE )
	{
		vkDestroyPipeline( device.handle, handle, nullptr );
	}
}


GraphicsPipeline::GraphicsPipeline( GraphicsPipeline&& other )
    : device { other.device }
    , layout { other.layout }
    , handle { other.handle }
    , index { other.index }
{
	other.handle = VK_NULL_HANDLE;
}


GraphicsPipeline& GraphicsPipeline::operator=( GraphicsPipeline&& other )
{
	assert( device.handle == other.device.handle && "Cannot move assign graphics line_pipeline of another device" );
	assert( layout.handle == other.layout.handle && "Cannot move pipelines with different layout" );
	std::swap( handle, other.handle );
	std::swap( index, other.index );
	return *this;
}


}  // namespace spot::gfx
