#
# This file is part of the aMule Project.
#
# Copyright (c) 2011 Werner Mahr (Vollstrecker) <amule@vollstreckernet.de>
#
# Any parts of this program contributed by third-party developers are copyrighted
# by their respective authors.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA
#
# This file contains the options for en- or disabling parts of aMule, and
# sets the needed variables for them to compile
#

option (BUILD_AMULECMD "compile aMule command line client")

option (BUILD_DAEMON "compile aMule daemon version")
option (BUILD_ED2K "compile aMule ed2k links handler" ON)
option (BUILD_EVERYTHING "compile all parts of aMule")
option (BUILD_MONOLITHIC "enable building of the monolithic aMule app" ON)

option (BUILD_REMOTEGUI "compile aMule remote GUI")
option (BUILD_WEBSERVER "compile aMule WebServer")
option (BUILD_TESTING "Run Tests after compile" ON)

if (PREFIX)
	set (CMAKE_INSTALL_PREFIX "${PREFIX}")
endif()

include (GNUInstallDirs)

set (PKGDATADIR "${CMAKE_INSTALL_DATADIR}/${PACKAGE}")

if (BUILD_EVERYTHING)
	set (BUILD_AMULECMD ON CACHE BOOL "compile aMule command line client" FORCE)
	set (BUILD_DAEMON ON CACHE BOOL "compile aMule daemon version" FORCE)
	set (BUILD_REMOTEGUI ON CACHE BOOL "compile aMule remote GUI" FORCE)
	set (BUILD_WEBSERVER ON CACHE BOOL "compile aMule WebServer" FORCE)
endif()

if (BUILD_AMULECMD)
	set (NEED_LIB_EC TRUE)
	set (NEED_LIB_MULECOMMON TRUE)
	set (NEED_LIB_MULESOCKET TRUE)
	set (wx_NEED_NET TRUE)
	set (NEED_ZLIB TRUE)
endif()

if (BUILD_DAEMON)
	set (NEED_LIB_EC TRUE)
	set (NEED_LIB_MULEAPPCOMMON TRUE)
	set (NEED_LIB_MULECOMMON TRUE)
	set (NEED_LIB_MULESOCKET TRUE)
	set (NEED_ZLIB TRUE)
	set (wx_NEED_NET TRUE)
endif()

if (BUILD_ED2K)
	set (wx_NEED_BASE TRUE)
endif()

if (BUILD_MONOLITHIC)
	set (NEED_LIB_EC TRUE)
	set (NEED_LIB_MULEAPPGUI TRUE)
	set (NEED_LIB_MULEAPPCOMMON TRUE)
	set (NEED_LIB_MULECOMMON TRUE)
	set (NEED_LIB_MULESOCKET TRUE)
	set (NEED_ZLIB TRUE)
	set (wx_NEED_NET TRUE)
endif()

if (BUILD_MONOLITHIC OR BUILD_REMOTEGUI)
	set (INSTALL_SKINS TRUE)
endif()

if (BUILD_REMOTEGUI)
	set (NEED_GLIB_CHECK TRUE)
	set (NEED_LIB_EC TRUE)
	set (NEED_LIB_MULEAPPCOMMON TRUE)
	set (NEED_LIB_MULEAPPGUI TRUE)
	set (NEED_LIB_MULECOMMON TRUE)
	set (NEED_LIB_MULESOCKET TRUE)
	set (NEED_ZLIB TRUE)
	set (wx_NEED_NET TRUE)
endif()

if (BUILD_WEBSERVER)
	set (NEED_LIB_EC TRUE)
	set (NEED_LIB_MULECOMMON TRUE)
	set (NEED_LIB_MULESOCKET TRUE)
	set (NEED_ZLIB TRUE)
	set (WEBSERVERDIR "${PKGDATADIR}/webserver/")
	set (wx_NEED_NET TRUE)
endif()

if (NEED_LIB_EC)
	set (NEED_LIB_CRYPTO TRUE)
endif()

if (NEED_LIB_MULECOMMON OR NEED_LIB_EC)
	set (NEED_LIB TRUE)
	set (wx_NEED_BASE TRUE)
endif()

if (NEED_LIB_MULECOMMON)
	set (NEED_GLIB_CHECK TRUE)
endif()

if (NEED_LIB_MULEAPPCOMMON)
	option (ENABLE_BOOST "compile with Boost.ASIO Sockets" ON)
	option (ENABLE_MMAP "enable using mapped memory if supported")
	option (ENABLE_NLS "enable national language support" ON)
	set (NEED_LIB_MULEAPPCORE TRUE)
	set (wx_NEED_BASE TRUE)
else()
	set (ENABLE_BOOST FALSE)
	set (ENABLE_MMAP FALSE)
	set (ENABLE_NLS FALSE)
endif()

if (NEED_LIB_MULEAPPGUI)
	set (wx_NEED_GUI TRUE)
endif()

if (NEED_LIB_MULESOCKET)
	set (wx_NEED_BASE TRUE)
endif()

if (ENABLE_BOOST AND NOT (BUILD_DAEMON OR BUILD_MONOLITHIC OR BUILD_REMOTEGUI))
	set (wx_NEED_NET FALSE)
endif()

if (wx_NEED_BASE OR wx_NEED_GUI OR wx_NEED_NET)
	set (wx_NEEDED TRUE)
endif()

ADD_COMPILE_DEFINITIONS ($<$<CONFIG:DEBUG>:__DEBUG__>)
