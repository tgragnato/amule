add_library (CRYPTOPP::CRYPTOPP
	UNKNOWN
	IMPORTED
)

find_path(CryptoPP_INCLUDE_DIR NAMES cryptopp/config.h DOC "CryptoPP include directory")
find_library(CryptoPP_LIBRARY NAMES cryptopp DOC "CryptoPP library")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CryptoPP
    REQUIRED_VARS CryptoPP_INCLUDE_DIR CryptoPP_LIBRARY
    FOUND_VAR CryptoPP_FOUND
    VERSION_VAR CRYPTOPP_VERSION)
set_target_properties(CRYPTOPP::CRYPTOPP PROPERTIES
    IMPORTED_LOCATION "${CryptoPP_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${CryptoPP_INCLUDE_DIR}")

mark_as_advanced(CryptoPP_INCLUDE_DIR CryptoPP_LIBRARY)
set(CRYPTOPP_INCLUDE_PREFIX ${CryptoPP_INCLUDE_DIR})
set(CRYTOPP_LIB_SEARCH_PATH ${CryptoPP_LIBRARY})

if (NOT CRYPTOPP_VERSION)
	set (CMAKE_CONFIGURABLE_FILE_CONTENT
		"#include <${CryptoPP_INCLUDE_DIR}/cryptopp/config.h>\n
		#include <stdio.h>\n
		int main(){\n
			printf (\"%d\", CRYPTOPP_VERSION);\n
		}\n"
	)

	configure_file ("${CMAKE_ROOT}/Modules/CMakeConfigurableFile.in"
		"${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/CheckCryptoppVersion.cxx" @ONLY IMMEDIATE
	)

	try_run (RUNRESULT
		COMPILERESULT
		${CMAKE_BINARY_DIR}
		${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/CheckCryptoppVersion.cxx
		RUN_OUTPUT_VARIABLE CRYPTOPP_VERSION
	)

	string (REGEX REPLACE "([0-9])([0-9])([0-9])" "\\1.\\2.\\3" CRYPTOPP_VERSION "${CRYPTOPP_VERSION}")

	if (${CRYPTOPP_VERSION} VERSION_LESS ${MIN_CRYPTOPP_VERSION})
		message (FATAL_ERROR "crypto++ version ${CRYPTOPP_VERSION} is too old")
	else()
		MESSAGE (STATUS "crypto++ version ${CRYPTOPP_VERSION} -- OK")
		set (CRYPTOPP_CONFIG_FILE ${CRYPTOPP_CONFIG_FILE} CACHE STRING "Path to config.h of crypto-lib" FORCE)
		set (CRYPTOPP_VERSION ${CRYPTOPP_VERSION} CACHE STRING "Version of cryptopp" FORCE)
	endif()
endif()
