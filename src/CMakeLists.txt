# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${VibrationDAQ_SOURCE_DIR}/include/vibration_daq/*.hpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(vibration_library ConfigModule.cpp VibrationSensorModule.cpp StorageModule.cpp ../lib/loguru/loguru.cpp ../lib/date/date.h ../lib/date/tz.cpp ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(vibration_library PUBLIC ../include)
target_include_directories(vibration_library PUBLIC ../lib)

# This depends on (header only) boost
#target_link_libraries(vibration_library Boost::boost)

# needed for file handling
target_link_libraries(vibration_library stdc++fs)

# needed for yaml-cpp library
include_directories(${YAML_INCLUDES})
target_link_libraries(vibration_library ${YAML_LIBRARIES})

# needed by loguru library
target_link_libraries(vibration_library Threads::Threads)
if(CMAKE_DL_LIBS)
	target_link_libraries(vibration_library ${CMAKE_DL_LIBS})
endif()

# download, build and add c-periphery library
# inspired by: https://mirkokiefer.com/cmake-by-example-f95eb47d45b1
include(ExternalProject)
ExternalProject_Add(periphery_lib
		URL https://github.com/vsergeev/c-periphery/archive/v2.1.0.tar.gz
		PREFIX ${CMAKE_CURRENT_BINARY_DIR}/c-periphery-2.1.0
		BUILD_IN_SOURCE 1
		CONFIGURE_COMMAND ""
		BUILD_COMMAND make
		INSTALL_COMMAND ""
		PREFIX=${CMAKE_CURRENT_BINARY_DIR}/c-periphery-2.1.0
		UPDATE_DISCONNECTED 1
		PATCH_COMMAND ""
		)
ExternalProject_Get_Property(periphery_lib install_dir)
add_library(periphery STATIC IMPORTED)
set_property(TARGET periphery PROPERTY IMPORTED_LOCATION ${install_dir}/src/periphery_lib/periphery.a)
target_link_libraries(vibration_library periphery)
add_dependencies(vibration_library periphery_lib)
target_include_directories(vibration_library PUBLIC ${install_dir}/src/periphery_lib/src/)

# All users of this library will need at least C++17
target_compile_features(vibration_library PUBLIC cxx_std_17)

# IDEs should put the headers in a nice place
source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})