project(resample)
SET(resample_VERSION 1)
SET(resample_SOVERSION 1.8)

SET(resample_HEADERS
	src/filterkit.h
	src/resample.h
	src/stdefs.h
)

SET(resample_SOURCES
	src/filterkit.c
)

ADD_LIBRARY(resample SHARED
	${resample_SOURCES}
)

SET_TARGET_PROPERTIES(resample PROPERTIES
	LINKER_LANGUAGE C
	VERSION ${resample_VERSION}
	SOVERSION ${resample_SOVERSION}
)

install(TARGETS resample DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(FILES ${resample_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/resample")
