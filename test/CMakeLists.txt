include_directories(${LIBIBUDDY_SOURCE_DIR}/src /usr/include /usr/local/include)
link_directories(${LIBIBUDDY_SOURCE_DIR}/src /usr/lib /usr/local/lib)

add_executable(ibuddy_test run_test.c)
target_link_libraries(ibuddy_test ibuddy)

install(TARGETS ibuddy_test DESTINATION bin)
