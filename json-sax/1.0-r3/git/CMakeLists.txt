project(json-sax)
SET(json-sax_VERSION 1)
SET(json-sax_SOVERSION 1.0)


SET(json-sax_HEADERS
	json.h
)

SET(json-sax_SOURCES
	json.c
)

ADD_LIBRARY(json-sax SHARED
	${json-sax_SOURCES}
)

SET_TARGET_PROPERTIES(json-sax PROPERTIES
	LINKER_LANGUAGE C
	VERSION ${json-sax_VERSION}
	SOVERSION ${json-sax_SOVERSION}
)

install(TARGETS json-sax DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(FILES ${json-sax_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/json-sax")
