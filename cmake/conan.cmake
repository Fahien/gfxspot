# Conan
# Download automatically, you can also just copy the conan.cmake file
if( NOT TARGET CONAN_PKG::glfw )
	if( NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake" )
		message( STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan" )
		file( DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
			"${CMAKE_BINARY_DIR}/conan.cmake" )
		endif()
	include( ${CMAKE_BINARY_DIR}/conan.cmake )
	conan_cmake_run( CONANFILE conanfile.txt BASIC_SETUP CMAKE_TARGETS BUILD missing )
endif()
