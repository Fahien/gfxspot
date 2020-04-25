# Add FileSpot
set( FILESPOT filespot )
set( FILESPOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../file )
set( FILESPOT_INCLUDE_DIR ${FILESPOT_DIR}/include )
set( FILESPOT_LIBRARY ${FILESPOT} )
if( ANDROID )
	set( FILESPOT_LIBRARY ${FILESPOT_LIBRARY} android )
endif()

set( FILESPOT_INCLUDE_DIRS ${FILESPOT_INCLUDE_DIR} )
set( FILESPOT_LIBRARIES ${FILESPOT_LIBRARY} )

# Check FileSpot
if( NOT TARGET ${FILESPOT} )
	add_subdirectory( ${FILESPOT_DIR} ${CMAKE_BINARY_DIR}/dep/file )
endif()
