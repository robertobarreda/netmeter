include(CheckIncludeFiles)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${LIBNETMETER_INCLUDE_DIR})

set(libnmsnooper_SRCS nmsnooper.cpp)

add_library(nmsnooper MODULE ${libnmsnooper_SRCS})

set_target_properties(nmsnooper PROPERTIES LOCATION ${NETMETER_PLUGIN_DIR}
   PREFIX "lib"
)

install(
    TARGETS nmsnooper
    LIBRARY DESTINATION ${NETMETER_PLUGIN_DIR}
)

