include_directories(${LIBIBUDDY_SOURCE_DIR}/src /usr/include /usr/local/include)
link_directories(${LIBIBUDDY_SOURCE_DIR}/src /usr/lib /usr/local/lib)
add_definitions( -DLIBIBUDDY_VERSION_STRING=\"0.0.1\" )

add_library(ibuddy SHARED  ibuddy.c util.c)
target_link_libraries(ibuddy usb)

set_target_properties  (ibuddy PROPERTIES
                        PUBLIC_HEADER "ibuddy.h")

install(TARGETS ibuddy
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include)
