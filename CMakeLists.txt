cmake_minimum_required( VERSION 3.15 )
project( gfxspot
	VERSION 0.2.0
	LANGUAGES C CXX )

# Dep
set( MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
list( APPEND CMAKE_MODULE_PATH ${MODULE_PATH} )
include( ${MODULE_PATH}/conan.cmake )

find_package( Vulkan )
include( AddCoreSpot )
include( AddMathSpot )
include( AddFileSpot )
#include( doxygen )

# Lib
set( SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/color.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/png.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/buffers.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/commands.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/descriptors.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/images.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/renderer.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/pipelines.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/models.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graphics.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/shader.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/glfw.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/animations.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/camera.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/viewport.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/animation.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/bounds.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/model.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/mesh.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/node.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/gui.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/gui_resources.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/collisions.cc
)
add_library( ${PROJECT_NAME} ${SOURCES} )
target_include_directories( ${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${Vulkan_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME}
	${Vulkan_LIBRARIES}
	CONAN_PKG::glfw
	CONAN_PKG::libpng
	CONAN_PKG::imgui
	corespot
	mathspot
	filespot )
target_compile_features( ${PROJECT_NAME} PUBLIC cxx_std_17 )

add_subdirectory( test )
