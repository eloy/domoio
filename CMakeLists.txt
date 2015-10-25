cmake_minimum_required (VERSION 2.8.8)
project (domoio)

option(PGSQL_SEARCH_PATH "Aditional paths where search for postgres libs")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set (PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Location for custm Find*.cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_ROOT}/cmake/Modules/")

# Setup includes
include_directories(./include ./vendor/include)

# V8
#-----------------------------------------------------------------------

find_package(V8)
set(DEP_LIBS ${DEP_LIBS} ${V8_LIBRARIES} -lpthread -lstdc++.6)

# ICU
find_package(ICU)
set(DEP_LIBS ${DEP_LIBS} ${ICU_LIBRARIES})

# Postgres
#-----------------------------------------------------------------------

# https://bugs.launchpad.net/ubuntu/+source/cmake/+bug/1317135
# "/usr/include/postgresql/9.3/server"
IF(PGSQL_SEARCH_PATH)
  SET(PostgreSQL_ADDITIONAL_SEARCH_PATHS ${PGSQL_SEARCH_PATH})
ENDIF(PGSQL_SEARCH_PATH)

find_package(PostgreSQL)
set(DEP_LIBS ${DEP_LIBS} ${PostgreSQL_LIBRARIES})
include_directories(${PostgreSQL_INCLUDE_DIRS})

# OpenSSL
#-----------------------------------------------------------------------

find_package(OpenSSL)
set(DEP_LIBS ${DEP_LIBS} ${OPENSSL_LIBRARIES})
include_directories(${OPENSSL_INCLUDE_DIR})

# BOOST
#-----------------------------------------------------------------------

include_directories(/usr/local/include/boost)
set(DEP_LIBS ${DEP_LIBS}
  -lboost_system -lboost_thread -lboost_program_options -lboost_log -lboost_log_setup
  -lboost_filesystem -lboost_date_time -lboost_regex)


set(DEP_LIBS ${DEP_LIBS}
  -lboost_program_options-mt -lboost_log-mt -lboost_regex-mt)


# MicroHttpd
#-----------------------------------------------------------------------
# set(DEP_LIBS ${DEP_LIBS} -lmicrohttpd)

# mongoose
#-----------------------------------------------------------------------
include_directories(vendor/mongoose)
set(SRC ${SRC} vendor/mongoose/mongoose)


# Add sources
#-----------------------------------------------------------------------

set(SRC ${SRC}
  ./src/domoio
  ./src/options
  ./src/server
  ./src/server_run
  ./src/connection
  ./src/device_connection
  ./src/ssdp.cc
  ./src/events
  ./src/database
  ./src/helpers
  ./src/crypto/crypto
  ./src/crypto/block_cipher
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
  ./src/commands/cmd_create_session
  ./src/commands/cmd_login
  ./src/commands/cmd_set
  ./src/commands/cmd_all_ports_state
)


# JS Engine
set(SRC ${SRC}
  ./src/jsengine/engine
  ./src/jsengine/device_template
  ./src/jsengine/port_template
  ./src/jsengine/devices_callback
  ./src/jsengine/log_callback
  ./src/jsengine/read_file_callback
  ./src/jsengine/file_exists_callback
  )

# HTTP Server
set(SRC ${SRC}
  ./src/httpd/http_server
  ./src/httpd/request
  ./src/httpd/httpd_action
  # ./src/httpd/actions/http_static_content
  ./src/httpd/actions/http_events
  # ./src/httpd/actions/http_console
  ./src/httpd/actions/http_devices
  ./src/httpd/actions/http_ports
  ./src/httpd/actions/http_users
  )

# Create the shared library
#-----------------------------------------------------------------------

add_library (domoio_base ${SRC})
target_link_libraries (domoio_base ${DEP_LIBS})


# Add the exectutable
#-----------------------------------------------------------------------

add_executable(domoio ./src/main)
target_link_libraries (domoio domoio_base ${DEP_LIBS})

# Test
#-----------------------------------------------------------------------

# Configure Google Test Lib
IF(GTEST_PATH)
  include_directories(${GTEST_PATH}/include)
  set(TEST_DEP_LIBS ${TEST_DEP_LIBS} -L${GTEST_PATH}/build-aux)
ENDIF(GTEST_PATH)
set(TEST_DEP_LIBS ${TEST_DEP_LIBS} -lgtest -lgtest_main)


# Support code
include_directories(./test/support)
set(TEST_SRC ${TEST_SRC}
  ./test/support/test_device
  ./test/support/factories
  ./test/support/helpers
)


# Test Sources
# Features
set(TEST_SRC ${TEST_SRC} ./test/domoio_test
  ./test/features/server_test
  ./test/features/events_test
  ./test/features/create_session_test
  ./test/features/signals_test
  ./test/features/cmd_set_test
  ./test/features/cmd_all_ports_state_test

  # # HTTP
  ./test/features/http/ports/update_test
)

# Unit
set(TEST_SRC ${TEST_SRC}
  ./test/unit/devices_test
  ./test/unit/block_cypher_test
  ./test/unit/httpd/server_test
  ./test/unit/httpd/request_test
  ./test/unit/httpd/httpd_action_test
  ./test/unit/models/model_user_test
  ./test/unit/models/model_device_test
)



add_executable(tests EXCLUDE_FROM_ALL ${TEST_SRC})
target_link_libraries (tests domoio_base ${DEP_LIBS} ${TEST_DEP_LIBS})


# INSTALL
#-----------------------------------------------------------------------
