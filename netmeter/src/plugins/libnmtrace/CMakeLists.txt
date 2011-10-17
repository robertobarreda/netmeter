include(CheckIncludeFiles)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${LIBNETMETER_INCLUDE_DIR})

set(libtrace_SRCS nmtrace.cpp)

add_library(nmtrace MODULE ${libtrace_SRCS})

set_target_properties(nmtrace PROPERTIES LOCATION ${NETMETER_PLUGIN_DIR}
   PREFIX "lib"
)

install(
    TARGETS nmtrace
    LIBRARY DESTINATION ${NETMETER_PLUGIN_DIR}
)

