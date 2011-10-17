include(CheckIncludeFiles)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${LIBNETMETER_INCLUDE_DIR})

set(libnmping_SRCS nmping.cpp)

add_library(nmping MODULE ${libnmping_SRCS})

set_target_properties(nmping PROPERTIES LOCATION ${NETMETER_PLUGIN_DIR}
   PREFIX "lib"
)

install(
    TARGETS nmping
    LIBRARY DESTINATION ${NETMETER_PLUGIN_DIR}
)

