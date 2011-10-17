include(CheckIncludeFiles)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${LIBNETMETER_INCLUDE_DIR})

set(libsync_SRCS nmsync.cpp)

add_library(nmsync MODULE ${libsync_SRCS})

set_target_properties(nmsync PROPERTIES LOCATION ${NETMETER_PLUGIN_DIR}
   PREFIX "lib"
)

install(
    TARGETS nmsync
    LIBRARY DESTINATION ${NETMETER_PLUGIN_DIR}
)

