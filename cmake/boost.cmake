find_package(Boost ${MIN_BOOST_VERSION} CONFIG REQUIRED)
# SYSTEM keeps our warning flags off Boost's headers.
include_directories (SYSTEM ${Boost_INCLUDE_DIRS})

include (CheckIncludeFiles)

set (CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIRS})

# Force a re-check every configure: without this an earlier
# missing-headers result lives forever in the CMakeCache, even
# after the user installs libboost-dev properly. Cheap (one
# compile of a 3-line test program) and avoids the cache-poison
# trap the previous version of this file had.
unset (ASIO_SOCKETS CACHE)
set (CMAKE_REQUIRED_FLAGS "-DBOOST_ERROR_CODE_HEADER_ONLY")
# Asio uses pthreads, which glibc before 2.34 keeps out of libc: without them the
# probe fails to link and reads as missing headers. Boost_LIBRARIES below copies
# this list, so the targets that use Asio link threads explicitly too.
set (THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package (Threads REQUIRED)
set (CMAKE_REQUIRED_LIBRARIES Threads::Threads)
check_include_files ("boost/system/error_code.hpp;boost/asio.hpp" ASIO_SOCKETS LANGUAGE CXX)

if (NOT ASIO_SOCKETS)
	# boost::asio is mandatory. The previous version of this file
	# silently downgraded to wxWidgets sockets when DOWNLOAD_AND_BUILD_DEPS
	# was set, which produced a build that compiled the long-deprecated
	# wx-sockets fallback path (issue #600). That path is being removed;
	# there is no fallback to fall back to.
	message (FATAL_ERROR "boost::asio headers were not usable. "
		"find_package(Boost) found a Boost install but "
		"check_include_files for boost/system/error_code.hpp + "
		"boost/asio.hpp failed to compile. Install full Boost "
		"development headers (Debian/Ubuntu: libboost-dev, Fedora: "
		"boost-devel, macOS Homebrew: boost, MSYS2: "
		"mingw-w64-clang-x86_64-boost) and re-configure. If the headers "
		"are clearly present, inspect build/CMakeFiles/CMakeError.log "
		"to see why the compile probe failed (typically a missing "
		"transitive dependency or a C++ standard mismatch).")
endif()

set (Boost_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} CACHE STRING "Libraries needed for linking with boost" FORCE)
# find_package(Boost CONFIG) above sets a local non-cache Boost_LIBRARIES
# that masks the cache value we just wrote — clear it so the cache shows through.
unset (Boost_LIBRARIES)
set (BOOST_ERROR_CODE_HEADER_ONLY TRUE CACHE INTERNAL "When true, boost_system lib is not needed for linking" FORCE)
unset (CMAKE_REQUIRED_INCLUDES)
unset (CMAKE_REQUIRED_FLAGS)
unset (CMAKE_REQUIRED_LIBRARIES)
