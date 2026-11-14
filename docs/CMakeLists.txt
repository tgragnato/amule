add_subdirectory (man)

install (FILES
	EC_Protocol.md
	EC_SRP.md
	IPv6_Server_Sources.md
	NAT_T_Protocol.md
	INSTALL.md
	LOGGING.md
	README.md
	THIRDPARTY.md
	amulesig.md
	DESTINATION "${CMAKE_INSTALL_DOCDIR}"
)
