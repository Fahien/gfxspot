# Find glslangValidator
find_program( GLSLV glslangValidator )
if( NOT GLSLV )
	message( FATAL_ERROR "glslangValidator not found" )
endif()

add_custom_target( shaders )

# Helper function to compile a shader
function( compile_shader SHADER_NAME )
	add_custom_command(
		TARGET shaders
		POST_BUILD
		COMMAND ${GLSLV} ${SHADER_NAME} -V -o ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.spv
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Compiling ${SHADER_NAME}" )
endfunction()

# List of shaders to compile
set( SHADERS
	line.vert
	line.frag
	mesh.vert
	mesh.frag
	mesh-no-image.vert
	mesh-no-image.frag
	imgui.vert
	imgui.frag
	quad.vert
	quad.frag
)

# Compile each shader
foreach( SHADER ${SHADERS} )
	compile_shader( ${SHADER} )
endforeach()

