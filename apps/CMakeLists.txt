add_executable(vibration_daq_app main.cpp)
target_compile_features(vibration_daq_app PRIVATE cxx_std_17)

target_link_libraries(vibration_daq_app PRIVATE vibration_library)

install(TARGETS vibration_daq_app
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)