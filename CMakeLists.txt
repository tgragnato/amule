cmake_minimum_required (VERSION 4.0)
project (amule)
set (MIN_BOOST_VERSION 1.47)
set (MIN_CRYPTOPP_VERSION 5.6)
set (MIN_GDLIB_VERSION 2.0.0)
set (MIN_WX_VERSION 3.2.0)
set (PACKAGE "amule")
set (PACKAGE_BUGREPORT "admin@amule.org")
set (PACKAGE_NAME "aMule")
set (PACKAGE_STRING "aMule SVN")
set (PACKAGE_TARNAME "amule")
set (PACKAGE_URL \"\")
set (PACKAGE_VERSION "SVN")
set (VERSION "GIT")
set (DEFAULT_BUILD_TYPE "Release")
set (RECONF_COMMAND ${CMAKE_COMMAND})

if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
	set (DEFAULT_BUILD_TYPE "Debug")
endif()

# Set the possible values of build type for cmake-gui
if (CMAKE_CONFIGURATION_TYPES)
	set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE
		STRING "Semicolon separated list of supported configuration types, only supports debug and release, anything else will be ignored" FORCE
	)

	set_property (CACHE CMAKE_CONFIGURATION_TYPES PROPERTY STRINGS
		"Debug" "Release"
	)
endif()

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message (STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
	set (CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE
		STRING "Choose the type of build." FORCE
	)
endif()

include (cmake/manpage_install.cmake)
include (cmake/options.cmake)
include (cmake/search-dirs.cmake)

if (BUILD_AMULECMD OR BUILD_WEBSERVER)
	include (cmake/FindReadline.cmake)
endif()

if (BUILD_WEBSERVER OR NEED_ZLIB)
	include (cmake/zlib.cmake)
endif()

if (BUILD_WEBSERVER)
	include (cmake/png.cmake)
endif()

if (ENABLE_BOOST)
	include (cmake/boost.cmake)
endif()

if (ENABLE_NLS)
	include (cmake/nls.cmake)
endif()

if (NEED_GLIB_CHECK)
	include (cmake/glib21.cmake)
endif()

if (NEED_LIB_CRYPTO)
	include (cmake/cryptopp.cmake)
endif()

if (wx_NEEDED)
	include (cmake/wx.cmake)
endif()

if (NOT SVNDATE)
	find_package (Git)

	if (GIT_FOUND)
		execute_process (
			COMMAND ${GIT_EXECUTABLE} describe
			OUTPUT_VARIABLE GIT_INFO_WC_REVISION
			OUTPUT_STRIP_TRAILING_WHITESPACE
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	endif (GIT_FOUND)

	set (SVNDATE "rev. ${GIT_INFO_WC_REVISION}" CACHE STRING "Revision to be written to version string" FORCE)
	message (STATUS "git revision ${SVNDATE} found")
endif()

include (cmake/bfd.cmake)

configure_file (
	config.h.cm
	config.h
)

if (BUILD_MONOLITHIC)
	install (FILES amule.desktop
		DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
	)
endif()

if (BUILD_REMOTEGUI)
	install (FILES amulegui.desktop
		DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
	)
endif()

if (ENABLE_NLS)
	include (FindGettext)
	add_subdirectory (po)
endif()

add_subdirectory (docs)
add_subdirectory (src)

if (BUILD_TESTING)
	enable_testing()
	add_subdirectory (unittests)
endif()

message (STATUS "

	Configured aMule ${PACKAGE_VERSION}${SVN_REVISION} for '${CMAKE_SYSTEM}' on '${CMAKE_SYSTEM_PROCESSOR}'.

	aMule enabled options:

	**** aMule Core ****
	Prefix where aMule should be installed?				${CMAKE_PREFIX_PATH}
	Should aMule be compiled with i18n support?			${ENABLE_NLS}
	Which mode should aMule be compiled in?				${CMAKE_BUILD_TYPE}
	Should aMule monolithic application be built?			${BUILD_MONOLITHIC}
	Should aMule daemon version be built?				${BUILD_DAEMON}
	Should aMule remote gui be built?				${BUILD_REMOTEGUI}

	**** aMule TextClient ****
	Should aMule Command Line Client be built?			${BUILD_AMULECMD}

	**** aMule WebServer ****
	Should aMule WebServer be built?				${BUILD_WEBSERVER}

	**** aMule ED2K Links Handler ****
	Should aMule ED2K Links Handler be built?			${BUILD_ED2K}

	Libraries aMule will use to build:"
)

if (NEED_WX)
	message (STATUS "			wxWidgets				${WX_VERSION}")
endif()

if (ASIO_SOCKETS)
	message ("			boost				${Boost_VERSION}")
endif()

if (NEED_LIB_CRYPTO)
	message ("			crypto++			${CRYPTOPP_VERSION} in ${CRYPTOPP_INCLUDE_PREFIX}")
endif()

message ("			libintl				${ENABLE_NLS}")

if (BUILD_WEBSERVER)
	message ("			libpng				${PNG_VERSION_STRING}")
endif()

if (NEED_ZLIB)
	message ("			zlib				${ZLIB_VERSION_STRING}")
endif()
