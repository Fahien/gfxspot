find_package(Doxygen)

doxygen_add_docs(${PROJECT_NAME}-doc
	${CMAKE_CURRENT_SOURCE_DIR}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating doxygen documentation")
