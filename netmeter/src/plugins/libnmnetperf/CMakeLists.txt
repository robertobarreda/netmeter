include(CheckIncludeFiles)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${LIBNETMETER_INCLUDE_DIR} ${LIBXML_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})

set(libnmnetperf_SRCS nmnetperf.cpp
                   nmnetperfexternaldecode.cpp
                   nmnetperfexternalnetperf.cpp
                   nmnetperfexternalnetserver.cpp
                   nmnetperfinterpret.cpp
)

add_library(nmnetperf MODULE ${libnmnetperf_SRCS})
target_link_libraries(nmnetperf ${LIBXML2_LIBRARIES})

set_target_properties(nmnetperf PROPERTIES LOCATION ${NETMETER_PLUGIN_DIR}
   PREFIX "lib"
)

install(
    TARGETS nmnetperf
    LIBRARY DESTINATION ${NETMETER_PLUGIN_DIR}
)

