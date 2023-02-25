add_executable (wxcas
	linuxmon.cpp
	onlinesig.cpp
	wxcas.cpp
	wxcascte.cpp
	wxcasframe.cpp
	wxcasprefs.cpp
	wxcaspix.cpp
	wxcasprint.cpp
)

target_include_directories (wxcas
	PRIVATE ${amule_BINARY_DIR}
)

target_link_libraries (wxcas
	wxWidgets::CORE
	wxWidgets::NET
)

install (TARGETS wxcas
	RUNTIME DESTINATION bin
)
