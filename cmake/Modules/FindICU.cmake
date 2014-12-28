#
# Try to find ICU library and include path.
# Once done this will define
#
# ICU_FOUND
# ICU_LIBRARY
#

IF(WIN32)
    # FIND_PATH( ICU_INCLUDE_PATH
		# $ENV{PROGRAMFILES}/ICU/include
		# ${ICU_LIB_DIR}/include
		# DOC "The directory where ICU/glfw3.h resides")

    # FIND_LIBRARY( ICU_LIBRARY
    #     NAMES glfw3 ICU
    #     PATHS
    #     $ENV{PROGRAMFILES}/ICU/lib
		# ${ICU_LIB_DIR}/lib
    #     DOC "The ICU library")
ELSE(WIN32)

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


	FIND_LIBRARY( ICU_I18_LIB
    NAMES libicui18n.a
		PATHS ${SEARCH_PATHS}
		DOC "The ICUI18 library")

  FIND_LIBRARY( ICU_UC_LIB
    NAMES libicuuc.a
		PATHS ${SEARCH_PATHS}
		DOC "The ICUUC library")

  	FIND_LIBRARY( ICU_DATA_LIB
      NAMES libicudata.a
			PATHS ${SEARCH_PATHS}
	    DOC "The ICUDATA library")

ENDIF(WIN32)

SET(ICU_FOUND "NO")
IF(ICU_I18_LIB AND ICU_DATA_LIB AND ICU_UC_LIB)
	SET(ICU_LIBRARIES ${ICU_I18_LIB} ${ICU_UC_LIB} ${ICU_DATA_LIB})
	SET(ICU_INCLUDE_DIRS ${ICU_INCLUDE_PATH})
	SET(ICU_FOUND "YES")
  MESSAGE(STATUS "Found ICU")
ELSE(ICU_I18_LIB AND ICU_DATA_LIB AND ICU_UC_LIB)
    message(STATUS "ICU NOT FOUND!!")
ENDIF(ICU_I18_LIB AND ICU_DATA_LIB AND ICU_UC_LIB)
