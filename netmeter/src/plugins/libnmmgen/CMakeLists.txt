include(CheckIncludeFiles)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${LIBNETMETER_INCLUDE_DIR} ${LIBXML_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})

add_subdirectory(xml)

find_package(LibXslt REQUIRED)

set(libnmmgen_SRCS nmmgen.cpp
   nmmgeninterpret.cpp
   nmmgenexternalmgen.cpp
   nmmgenexternaldrec.cpp
   nmmgenexternaldecode.cpp
   nmmgenexternaldistr.cpp
)

add_library(nmmgen MODULE ${libnmmgen_SRCS})
target_link_libraries(nmmgen ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES})

set_target_properties(nmmgen PROPERTIES LOCATION ${NETMETER_PLUGIN_DIR}
   PREFIX "lib"
)

install(
    TARGETS nmmgen
    LIBRARY DESTINATION ${NETMETER_PLUGIN_DIR}
)

