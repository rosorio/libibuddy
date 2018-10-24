include_directories(${LIBIBUDDY_SOURCE_DIR}/src /usr/include /usr/local/include)
link_directories(${LIBIBUDDY_SOURCE_DIR}/src /usr/lib /usr/local/lib)
add_definitions( -DLIBIBUDDY_VERSION_STRING=\"0.0.1\" )

add_executable(run_test run_test.c)
target_link_libraries(run_test ibuddy)





