cmake_minimum_required(VERSION 2.8.12)

find_package(PkgConfig)
pkg_check_modules(DBUSCPP REQUIRED dbus-1>=0.93)

set(dbus-c++_SOURCES
	src/connection.cpp
	src/debug.cpp
	src/dispatcher.cpp
	src/error.cpp
	src/eventloop.cpp
	src/eventloop-integration.cpp
	src/interface.cpp
	src/introspection.cpp
	src/message.cpp
	src/object.cpp
	src/pendingcall.cpp
	src/pipe.cpp
	src/property.cpp
	src/server.cpp
	src/types.cpp
)

file (GLOB_RECURSE dbus-c++_HEADERS include/dbus-c++/*)

add_library(dbus-c++-1
	SHARED
	${dbus-c++_SOURCES}
	${dbus-c++_HEADERS}
)

find_package(PkgConfig)
pkg_check_modules(DBUS1 REQUIRED dbus-1)

include_directories(include)
include_directories(./)
include_directories(${DBUS1_INCLUDE_DIRS})

add_definitions(-DHAVE_CONFIG_H)

target_link_libraries(dbus-c++-1
	pthread
	${DBUS1_LIBRARIES}
)

set_target_properties(dbus-c++-1 PROPERTIES VERSION "0.9.0" SOVERSION 0)

set (prefix ${CMAKE_INSTALL_PREFIX})
set (libdir ${CMAKE_INSTALL_LIBDIR})
set (includedir ${CMAKE_INSTALL_INCLUDEDIR})

configure_file("dbus-c++-1.pc.in" "dbus-c++-1.pc" @ONLY)

install(TARGETS dbus-c++-1 DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/dbus-c++-1.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
