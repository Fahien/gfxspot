# Add GltfSpot
set( GLTFSPOT gltfspot )
set( GLTFSPOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../gltf )
set( GLTFSPOT_INCLUDE_DIR ${GLTFSPOT_DIR}/include )
set( GLTFSPOT_LIBRARY ${GLTFSPOT} ß)
if( ANDROID )
	set( GLTFSPOT_LIBRARY ${GLTFSPOT_LIBRARY} m )
endif()

set( GLTFSPOT_INCLUDE_DIRS ${GLTFSPOT_INCLUDE_DIR} )
set( GLTFSPOT_LIBRARIES ${GLTFSPOT_LIBRARY} )

# Check GltfSpot
if( NOT TARGET ${GLTFSPOT} )
	add_subdirectory( ${GLTFSPOT_DIR} ${CMAKE_BINARY_DIR}/dep/gltf )
endif()
