# Add MathSpot
set(MATHSPOT mathspot)
set(MATHSPOT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/math/include)
set(MATHSPOT_LIBRARY ${MATHSPOT})
if(ANDROID)
	set(MATHSPOT_LIBRARY ${MATHSPOT_LIBRARY} m)
endif()

set(MATHSPOT_INCLUDE_DIRS ${MATHSPOT_INCLUDE_DIR})
set(MATHSPOT_LIBRARIES ${MATHSPOT_LIBRARY})

# Check MathSpot
if(NOT TARGET ${MATHSPOT})
	add_subdirectory(${CMAKE_SOURCE_DIR}/dep/math)
endif()
