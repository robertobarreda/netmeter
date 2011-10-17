add_subdirectory(plugins)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR})

ADD_EXECUTABLE(netmeter_bin netmeter.cpp)
target_link_libraries(netmeter_bin m dl pthread util ${LIBNETMETER_LIBRARY})

set_target_properties(netmeter_bin PROPERTIES
  INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
  INSTALL_RPATH_USE_LINK_PATH true
  OUTPUT_NAME netmeter
)

install(
    TARGETS netmeter_bin
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
