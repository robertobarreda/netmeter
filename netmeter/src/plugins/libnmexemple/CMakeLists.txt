include(CheckIncludeFiles)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${LIBNETMETER_INCLUDE_DIR})

set(libexemple_SRCS nmexemple.cpp)
set(libexempleadder_SRCS nmexempleadder.cpp)

add_library(nmexemple MODULE ${libexemple_SRCS})
add_library(nmexempleadder MODULE ${libexempleadder_SRCS})

set_target_properties(nmexempleadder PROPERTIES LOCATION ${NETMETER_PLUGIN_DIR}
   PREFIX "lib"
)

set_target_properties(nmexemple PROPERTIES LOCATION ${NETMETER_PLUGIN_DIR}
   PREFIX "lib"
)

install(
    TARGETS nmexemple nmexempleadder
    LIBRARY DESTINATION ${NETMETER_PLUGIN_DIR}
)

