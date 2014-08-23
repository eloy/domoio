cmake_minimum_required (VERSION 2.8.8)
project (domoio)


set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -std=gnu++0x -stdlib=libc++" )
set (PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Location for custm Find*.cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_ROOT}/cmake/Modules/")




# Setup includes
include_directories(./include)

# V8
find_package(V8)
set(DEP_LIBS ${DEP_LIBS} ${V8_LIBRARIES})


# Postgres
set(DEP_LIBS ${DEP_LIBS} -lpq)

# OpenSSL
find_package(openssl)
set(DEP_LIBS ${DEP_LIBS} ${OPENSSL_LIBRARIES})
include_directories(${OPENSSL_INCLUDE_DIR})

# BOOST
include_directories(/Users/harlock/src/c/boost)
set(DEP_LIBS ${DEP_LIBS} -L/Users/harlock/src/c/boost/stage/lib -lboost_system -lboost_thread)


# Add sources

set(SRC ${SRC}
  ./src/server
  ./src/server_run
  ./src/device_connection
  ./src/database
  ./src/crypto/aes
)

# Models
set(SRC ${SRC}
  ./src/models/device
  ./src/models/port
)

# Commands
set(SRC ${SRC}
  ./src/commands/cmd_exit
)


# JS Engine
set(SRC ${SRC} ./src/jsengine/engine)



# Create the shared library
add_library (domoio_base SHARED ${SRC})
target_link_libraries (domoio_base ${DEP_LIBS})


# Add the exectutable
add_executable(domoio ./src/main)
target_link_libraries (domoio domoio_base ${DEP_LIBS})


# Test
#-----------------------------------------------------------------------

# Test Sources
# Features
set(TEST_SRC ${TEST_SRC} ./test/domoio_test
  ./test/features/server_test
  ./test/features/login_test
)

# Unit
set(TEST_SRC ${TEST_SRC}
  ./test/unit/devices_test
  ./test/unit/block_cypher_test
)

# Configure Google Test Lib
IF(GTEST_PATH)
  include_directories(${GTEST_PATH}/include)
  set(TEST_DEP_LIBS ${TEST_DEP_LIBS} -L${GTEST_PATH}/build-aux)
ENDIF(GTEST_PATH)
set(TEST_DEP_LIBS ${TEST_DEP_LIBS} -lgtest -lgtest_main)




# Support code
include_directories(./test/support)
set(TEST_SRC ${TEST_SRC} ./test/support/test_device)

add_executable(test EXCLUDE_FROM_ALL ${TEST_SRC})
target_link_libraries (test domoio_base ${DEP_LIBS} ${TEST_DEP_LIBS})
