cmake_minimum_required (VERSION 2.8.8)
project (domoio)


set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -std=gnu++0x -stdlib=libc++" )
set (PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Location for custm Find*.cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_ROOT}/cmake/Modules/")


# Setup includes
include_directories(./include)

find_package(V8)

set(DEP_LIBS ${DEP_LIBS} ${V8_LIBRARIES})
message(STATUS ${V8_LIBRARIES})

# JS Engine
set(SRC ${SRC} ./src/jsengine/engine)


# Create the shared library
add_library (domoio_base SHARED ${SRC})
target_link_libraries (domoio_base ${DEP_LIBS})


# Add the exectutable
add_executable(domoio ./src/main)
target_link_libraries (domoio domoio_base ${DEP_LIBS})
