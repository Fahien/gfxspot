# Add GltfSpot
set(GLTFSPOT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/gltf/include)
set(GLTFSPOT_LIBRARY gltfspot)
if(ANDROID)
	set(GLTFSPOT_LIBRARY ${GLTFSPOT_LIBRARY} m)
endif()

set(GLTFSPOT_INCLUDE_DIRS ${GLTFSPOT_INCLUDE_DIR})
set(GLTFSPOT_LIBRARIES ${GLTFSPOT_LIBRARY})

# Check GltfSpot
if(NOT TARGET gltfspot)
	add_subdirectory(${CMAKE_SOURCE_DIR}/dep/gltf ${CMAKE_BINARY_DIR}/dep/gltf)
endif()
