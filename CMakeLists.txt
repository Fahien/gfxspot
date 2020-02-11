cmake_minimum_required( VERSION 3.15 )
project( gfxspot
	VERSION 0.1.0
	LANGUAGES C CXX )

# Dep
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

find_package( Vulkan )
find_package( GLFW )
find_package( PNG )
include( AddMathSpot )
include( AddGltfSpot )

# Lib
set( SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/png.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/buffers.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/commands.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/descriptors.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/images.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/renderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/pipelines.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/models.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graphics.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/glfw.cpp
)
add_library( ${PROJECT_NAME} ${SOURCES} )
target_include_directories( ${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${Vulkan_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} ${Vulkan_LIBRARIES} ${GLFW_LIBRARY} PNG::PNG mathspot gltfspot )
target_compile_features( ${PROJECT_NAME} PUBLIC cxx_std_17 )

function( add_demo NUM )
	set( DEMO_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test/demo-${NUM}.cpp )
	add_executable( ${PROJECT_NAME}-demo-${NUM} ${DEMO_SOURCES} )
	target_link_libraries( ${PROJECT_NAME}-demo-${NUM} ${PROJECT_NAME} )
	target_compile_features( ${PROJECT_NAME}-demo-${NUM} PUBLIC cxx_std_17 )
endfunction()

add_demo( 1 )
add_demo( 2 )
add_demo( 3 )
add_demo( 4 )
add_demo( 5 )

include( shaders )
