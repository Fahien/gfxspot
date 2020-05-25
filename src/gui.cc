#include "spot/gfx/gui.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "spot/gfx/glfw.h"


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


void char_callback( GLFWwindow* window, unsigned int c )
{
	auto& io = ImGui::GetIO();
	io.AddInputCharacter( c );
}


Gui::Gui( Device& device, Window& w )
    : images { device }
    , window { w }
    , vert { device, "res/shader/imgui.vert.spv" }
    , frag { device, "res/shader/imgui.frag.spv" }
    , layout { device, get_imgui_bindings(), get_constants() }
    , sampler { device }
{
	ImGui::CreateContext();

	// Display and style
	auto& io = ImGui::GetIO();
	io.DisplaySize.x = float( window.extent.width );
	io.DisplaySize.y = float( window.extent.height );
	io.FontGlobalScale = 2.0f;
	auto& style = ImGui::GetStyle();
	style.ScaleAllSizes( 2.0f );

	// Create font texture
	unsigned char* font_data;
	int            tex_width, tex_height;
	ImGui::GetIO().Fonts->GetTexDataAsRGBA32( &font_data, &tex_width, &tex_height );
	size_t upload_size = tex_width * tex_height * 4 * sizeof( char );

	VkExtent2D font_extent = { uint32_t( tex_width ), uint32_t( tex_height ) };
	font_view = images.load( "default-font", font_data, upload_size, font_extent );

	// Window keys
	glfwSetCharCallback( window.handle, char_callback );
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
}


Gui::~Gui()
{
	ImGui::DestroyContext();
}


void Gui::update( const float delta_time )
{
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = delta_time;

	VkViewport gui_viewport = {};
	gui_viewport.width = io.DisplaySize.x;
	gui_viewport.height = io.DisplaySize.y;

	auto cursor = window.cursor_to( gui_viewport );
	io.MousePos = ImVec2( cursor.x, gui_viewport.height - cursor.y );
	io.MouseDown[0] = window.press.left;
	io.MouseDown[1] = window.press.right;
	io.MouseDown[2] = window.press.middle;

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