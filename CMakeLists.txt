cmake_minimum_required( VERSION 3.15 )
project( gfxspot
	VERSION 0.1.0
	LANGUAGES C CXX )

# Dep
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

# Conan
# Download automatically, you can also just copy the conan.cmake file
if( NOT TARGET CONAN_PKG::fmt )
		if( NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake" )
		message( STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan" )
		file( DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
			"${CMAKE_BINARY_DIR}/conan.cmake" )
	endif()
	include( ${CMAKE_BINARY_DIR}/conan.cmake )
	conan_cmake_run( CONANFILE conanfile.txt BASIC_SETUP CMAKE_TARGETS BUILD missing )
endif()

find_package( Vulkan )

include( AddMathSpot )
include( AddGltfSpot )
#include( doxygen )

# Lib
set( SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/hash.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/png.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/buffers.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/commands.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/descriptors.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/images.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/renderer.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/pipelines.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/models.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graphics.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/glfw.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/animations.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/camera.cc
)
add_library( ${PROJECT_NAME} ${SOURCES} )
target_include_directories( ${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${Vulkan_INCLUDE_DIRS} )
target_link_libraries( ${PROJECT_NAME} ${Vulkan_LIBRARIES} CONAN_PKG::fmt CONAN_PKG::glfw CONAN_PKG::libpng mathspot gltfspot )
target_compile_features( ${PROJECT_NAME} PUBLIC cxx_std_17 )

add_subdirectory( test )
