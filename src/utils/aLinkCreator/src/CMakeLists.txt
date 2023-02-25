if (BUILD_ALCC)
	add_executable (alcc
		alcc.cpp
		ed2khash.cpp
		md4.cpp
	)

	target_include_directories (alcc
		PRIVATE ${amule_BINARY_DIR}
	)

	target_compile_definitions (alcc
		PRIVATE wxUSE_GUI=0
	)

	target_link_libraries (alcc
		wxWidgets::BASE
	)

	install (TARGETS alcc
		RUNTIME DESTINATION bin
	)
endif()

if (BUILD_ALC)
	add_executable (alc
		alc.cpp
		alcframe.cpp
		alcpix.cpp
		ed2khash.cpp
		md4.cpp
	)

	target_include_directories (alc
		PRIVATE ${amule_BINARY_DIR}
	)

	target_link_libraries (alc
		wxWidgets::CORE
	)

	install (TARGETS alc
		RUNTIME DESTINATION bin
	)
endif()
