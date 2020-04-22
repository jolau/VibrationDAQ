# according to https://gitlab.com/CLIUtils/modern-cmake/-/tree/master/examples/extended-project

cmake_minimum_required(VERSION 3.13)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

project(VibrationDAQ
		VERSION 0.1
		LANGUAGES CXX)


# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(CMAKE_CXX_STANDARD 17)

    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDE's
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Adds Boost::boost
#find_package(Boost REQUIRED)

# adds yaml-cpp library
find_package(Yaml REQUIRED)

# needed by loguru library
# https://stackoverflow.com/questions/1620918/cmake-and-libpthread
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DLOGURU_WITH_STREAMS=1")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# The compiled library code is here
add_subdirectory(src)
#add_subdirectory(lib)

# The executable code is here
add_subdirectory(apps)