include (FindBoost)

if (NOT ASIO_SOCKETS)
	include (CheckIncludeFiles)


	if (Boost_FOUND)
		set (CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIRS})
		set (CMAKE_REQUIRED_FLAGS "-DBOOST_ERROR_CODE_HEADER_ONLY")

		check_include_files ("boost/system/error_code.hpp;boost/asio.hpp" ASIO_SOCKETS LANGUAGE CXX)

		if (ASIO_SOCKETS)
			set (Boost_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} CACHE STRING "Libraries needed for linking with boost" FORCE)
			set (BOOST_ERROR_CODE_HEADER_ONLY TRUE CACHE INTERNAL "When true, boost_system lib is not needed for linking" FORCE)
			unset (CMAKE_REQUIRED_INCLUDES)
			unset (CMAKE_REQUIRED_FLAGS)
			unset (CMAKE_REQUIRED_LIBRARIES)
		else()
			message (STATUS "No useable boost headers found. Disabling support")
			set (ENABLE_BOOST FALSE)
		endif()
	else()
		message (STATUS "No useable boost headers found. Disabling support")
		set (ENABLE_BOOST FALSE)
	endif()
endif()
