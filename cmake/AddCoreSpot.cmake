# Add CoreSpot
set( CORESPOT corespot )
set( CORESPOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../core )
set( CORESPOT_INCLUDE_DIR ${CORESPOT_DIR}/include )
set( CORESPOT_LIBRARY ${CORESPOT} )

set( CORESPOT_INCLUDE_DIRS ${CORESPOT_INCLUDE_DIR} )
set( CORESPOT_LIBRARIES ${CORESPOT_LIBRARY} )

# Check CoreSpot
if( NOT TARGET ${CORESPOT} )
	add_subdirectory( ${CORESPOT_DIR} ${CMAKE_BINARY_DIR}/dep/core )
endif()
