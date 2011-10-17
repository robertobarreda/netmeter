include(CheckIncludeFiles)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${LIBNETMETER_INCLUDE_DIR})

set(libsimulator_SRCS nmsimulator.cpp)

add_library(nmsimulator MODULE ${libsimulator_SRCS})

set_target_properties(nmsimulator PROPERTIES LOCATION ${NETMETER_PLUGIN_DIR}
   PREFIX "lib"
)


install(
    TARGETS nmsimulator
    LIBRARY DESTINATION ${NETMETER_PLUGIN_DIR}
)

