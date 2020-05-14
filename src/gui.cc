#include "spot/gfx/gui.h"

#include <imgui.h>


namespace spot::gfx
{
std::vector<VkDescriptorSetLayoutBinding> get_imgui_bindings()
{
	VkDescriptorSetLayoutBinding sampler = {};
	sampler.binding = 0;
	sampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler.descriptorCount = 1;
	sampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	return { sampler };
}


VkPushConstantRange get_constants()
{
	VkPushConstantRange constants = {};
	constants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	constants.size = sizeof( Constants );
	return constants;
}


Gui::Gui( Device& device, const VkExtent2D& extent )
    : images { device }
    , vert { device, "shader/imgui.vert.spv" }
    , frag { device, "shader/imgui.frag.spv" }
    , layout { device, get_imgui_bindings(), get_constants() }
    , sampler { device }
{
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	io.DisplaySize.x = float( extent.width );
	io.DisplaySize.y = float( extent.height );

	// Create font texture
	unsigned char* font_data;
	int            tex_width, tex_height;
	ImGui::GetIO().Fonts->GetTexDataAsRGBA32( &font_data, &tex_width, &tex_height );
	size_t upload_size = tex_width * tex_height * 4 * sizeof( char );

	VkExtent2D font_extent = { uint32_t( tex_width ), uint32_t( tex_height ) };
	font_view = images.load( "default-font", font_data, upload_size, font_extent );
}


Gui::~Gui()
{
	ImGui::DestroyContext();
}


void Gui::update( const float delta_time )
{
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = delta_time;
	ImGui::Render();
}


std::pair<std::vector<ImDrawVert>, std::vector<ImDrawIdx>> gui_draw()
{
	ImDrawData* draw_data = ImGui::GetDrawData();
	if ( !draw_data )
	{
		return {};
	}

	// Get size
	size_t vertex_size = draw_data->TotalVtxCount;
	size_t index_size = draw_data->TotalIdxCount;
	if ( !vertex_size || !index_size )
	{
		return {};
	}

	// Make buffers
	std::vector<ImDrawVert> vertex_data( vertex_size );
	std::vector<ImDrawIdx> index_data( index_size );

	// Copy into buffers just creates
	ImDrawVert* vtx_dst = vertex_data.data();
	ImDrawIdx*  idx_dst = index_data.data();
	for ( int i = 0; i < draw_data->CmdListsCount; ++i )
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[i];
		memcpy( vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof( ImDrawVert ) );
		memcpy( idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof( ImDrawIdx ) );
		vtx_dst += cmd_list->VtxBuffer.Size;
		idx_dst += cmd_list->IdxBuffer.Size;
	}

	return { vertex_data, index_data };
}

}  // namespace spot::gfx