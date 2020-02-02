# Find glslangValidator
find_program( GLSLV glslangValidator )
if( NOT GLSLV )
	message( FATAL_ERROR "glslangValidator not found" )
endif()

add_custom_target( shaders )
add_dependencies( ${PROJECT_NAME}-test shaders )

# Helper function to compile a shader
function( compile_shader SHADER_NAME )
	add_custom_command(
		TARGET shaders
		POST_BUILD
		COMMAND ${GLSLV} ${SHADER_NAME} -V -o ${SHADER_NAME}.spv
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Compiling ${SHADER_NAME}"
	)
endfunction()

# List of shaders to compile
set( SHADERS
	${CMAKE_CURRENT_SOURCE_DIR}/test/shader/line.vert
	${CMAKE_CURRENT_SOURCE_DIR}/test/shader/line.frag
	${CMAKE_CURRENT_SOURCE_DIR}/test/shader/mesh.vert
	${CMAKE_CURRENT_SOURCE_DIR}/test/shader/mesh.frag
)

# Compile each shader
foreach( SHADER ${SHADERS} )
	compile_shader( ${SHADER} )
endforeach()
