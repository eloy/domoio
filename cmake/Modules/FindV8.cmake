#
# Try to find V8 library and include path.
# Once done this will define
#
# V8_FOUND
# V8_INCLUDE_PATH
# V8_LIBRARY
#

set(V8_INCLUDE_NAMES v8.h v8-debug.h v8-profiler.h)
set(V8_LIBRARY_NAMES )

IF(WIN32)
    # FIND_PATH( V8_INCLUDE_PATH
		# $ENV{PROGRAMFILES}/V8/include
		# ${V8_LIB_DIR}/include
		# DOC "The directory where V8/glfw3.h resides")

    # FIND_LIBRARY( V8_LIBRARY
    #     NAMES glfw3 V8
    #     PATHS
    #     $ENV{PROGRAMFILES}/V8/lib
		# ${V8_LIB_DIR}/lib
    #     DOC "The V8 library")
ELSE(WIN32)
  FIND_PATH( V8_INCLUDE_PATH ${V8_INCLUDE_NAMES}
		${V8_INCLUDE_DIR}
		/usr/include
		/usr/local/include
		/sw/include
    /opt/local/include
		DOC "V8 include dir")


  SET(SEARCH_PATHS
    ${V8_LIB_DIR}/
	  ${V8_LIB_DIR}/lib
	  /usr/lib64
	  /usr/lib
	  /usr/local/lib64
	  /usr/local/lib
	  /sw/lib
	  /opt/local/lib
    )


	FIND_LIBRARY( V8_LIBRARY
    NAMES libv8_base.a libv8_base.x64.a
		PATHS ${SEARCH_PATHS}
		DOC "The V8 library")


	FIND_LIBRARY( V8_SNAPSHOT_LIBRARY
    NAMES libv8_snapshot.a libv8_snapshot.x64.a
		PATHS ${SEARCH_PATHS}
		DOC "The V8 library")

ENDIF(WIN32)

SET(V8_FOUND "NO")
IF(V8_INCLUDE_PATH AND V8_LIBRARY AND V8_SNAPSHOT_LIBRARY)
	SET(V8_LIBRARIES ${V8_LIBRARY} ${V8_SNAPSHOT_LIBRARY})
	SET(V8_INCLUDE_DIRS ${V8_INCLUDE_PATH})
	SET(V8_FOUND "YES")
  MESSAGE(STATUS "Found V8")

ELSE(V8_INCLUDE_PATH AND V8_LIBRARY AND V8_SNAPSHOT_LIBRARY)
    message(STATUS "V8 NOT FOUND!!")
ENDIF(V8_INCLUDE_PATH AND V8_LIBRARY AND V8_SNAPSHOT_LIBRARY)


FIND_LIBRARY( V8_BASE_LIBRARY
        NAMES libv8_libbase.a libv8_libbase.x64.a
		    PATHS ${SEARCH_PATHS}
		    DOC "The V8 Base library")

IF(V8_BASE_LIBRARY)
  SET(V8_LIBRARIES ${V8_LIBRARIES} ${V8_BASE_LIBRARY})
  message(STATUS "Found V8 libv8_base")
ENDIF(V8_BASE_LIBRARY)