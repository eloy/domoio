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
		# ${V8_ROOT_DIR}/include
		# DOC "The directory where V8/glfw3.h resides")

    # FIND_LIBRARY( V8_LIBRARY
    #     NAMES glfw3 V8
    #     PATHS
    #     $ENV{PROGRAMFILES}/V8/lib
		# ${V8_ROOT_DIR}/lib
    #     DOC "The V8 library")
ELSE(WIN32)
  FIND_PATH( V8_INCLUDE_PATH ${V8_INCLUDE_NAMES}
		${V8_ROOT_DIR}/include
		/usr/include
		/usr/local/include
		/sw/include
    /opt/local/include
		DOC "V8 include dir")


	FIND_LIBRARY( V8_LIBRARY
        NAMES libv8_base.a libv8_base.x64.a
		PATHS
		${V8_ROOT_DIR}/lib
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		DOC "The V8 library")

	FIND_LIBRARY( V8_SNAPSHOT_LIBRARY
        NAMES libv8_snapshot.a libv8_snapshot.x64.a
		PATHS
		${V8_ROOT_DIR}/lib
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib

		DOC "The V8 library")

ENDIF(WIN32)

SET(V8_FOUND "NO")
IF(V8_INCLUDE_PATH AND V8_LIBRARY AND V8_SNAPSHOT_LIBRARY)
	SET(V8_LIBRARIES ${V8_LIBRARY} ${V8_SNAPSHOT_LIBRARY})
	SET(V8_INCLUDE_DIRS ${V8_INCLUDE_PATH})
	SET(V8_FOUND "YES")
    message(STATUS "Found V8")
    message(STATUS ${V8_LIBRARIES}   )
ELSE(V8_INCLUDE_PATH AND V8_LIBRARY AND V8_SNAPSHOT_LIBRARY)
    message(STATUS "V8 NOT FOUND!!")
    # message(STATUS ${V8_INCLUDE})
    # message(STATUS ${V8_LIBRARY})
ENDIF(V8_INCLUDE_PATH AND V8_LIBRARY AND V8_SNAPSHOT_LIBRARY)
