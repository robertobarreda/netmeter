PROJECT(netmetergui)

cmake_minimum_required(VERSION 2.4.8)

if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeOptions.txt)
        include(${CMAKE_SOURCE_DIR}/CMakeOptions.txt)
else(EXISTS ${CMAKE_SOURCE_DIR}/CMakeOptions.txt)
        message(FATAL_ERROR "CMakeOptions.txt not found! Run configure.cmake first.")
endif(EXISTS ${CMAKE_SOURCE_DIR}/CMakeOptions.txt)

#if you don't want the full compiler output, remove the following line
#SET(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(LibXml2 REQUIRED) # find and setup libXML2 for this project
find_package(LibXslt REQUIRED) # find and setup libXML2 for this project
find_package(Qt4 REQUIRED) # find and setup QT4 for this project
find_package(ZLIB REQUIRED) # find and setup Zlib for this project

include(QtMacros)
include(LtMacros)

find_package(LibNetMeter REQUIRED)

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall ${LIBXML2_DEFINITIONS} ${QT_DEFINITIONS} -DQT_THREAD_SUPPORT -DHAVE_CONFIG_H -g)

#need to link to some other libraries ? just add them here

include_directories(${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR} ${QT_INCLUDE_DIR} ${QT_INCLUDES} ${ZLIB_INCLUDE_DIR})
link_directories(${QT_LIB_DIR})

set(NETMETER_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib/netmetergui/plugins)
set(NETMETER_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
set(XML_DIR ${CMAKE_INSTALL_PREFIX}/share/netmetergui/xml)
set(SCRIPTS_DIR ${CMAKE_INSTALL_PREFIX}/share/netmetergui/scripts)
set(PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib/netmetergui/plugins)
set(ICON_DIR ${CMAKE_INSTALL_PREFIX}/share/netmeter/gui/icons)

add_subdirectory(src)
add_subdirectory(images)

configure_file(
    ${CMAKE_SOURCE_DIR}/config.h.cmake
    ${CMAKE_SOURCE_DIR}/config.h
)
