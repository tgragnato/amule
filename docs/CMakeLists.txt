add_subdirectory (man)

install (FILES
	ABOUT-NLS
	amulesig.txt
	Changelog
	EC_Protocol.txt
	DESTINATION "${CMAKE_INSTALL_DOCDIR}"
)
