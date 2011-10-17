PROJECT(netmeter)

cmake_minimum_required(VERSION 2.4.8)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeOptions.txt)
        include(${CMAKE_SOURCE_DIR}/CMakeOptions.txt)
else(EXISTS ${CMAKE_SOURCE_DIR}/CMakeOptions.txt)
        message(FATAL_ERROR "CMakeOptions.txt not found! Run configure.cmake first.")
endif(EXISTS ${CMAKE_SOURCE_DIR}/CMakeOptions.txt)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

#if you don't want the full compiler output, remove the following line
#SET(CMAKE_VERBOSE_MAKEFILE ON)

#SET (CMAKE_CXX_COMPILER g++-3.3)

find_package(LibNetMeter REQUIRED)

#set(NETMETER_CORE_DIR ${CMAKE_SOURCE_DIR}/src/core)

find_package(LibXml2 REQUIRED) # find and setup libXML2 for this project

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-g -Wall -O2 ${LIBXML2_DEFINITIONS} -DHAVE_CONFIG_H)

#need to link to some other libraries ? just add them here

include_directories(${LIBXML2_INCLUDE_DIR})

if(NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX /usr/local/netmeter)
endif(NOT CMAKE_INSTALL_PREFIX)

set(NETMETER_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib/netmeter/plugins)
set(XML_DIR ${CMAKE_INSTALL_PREFIX}/share/netmeter/xml)
set(PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib/netmeter/plugins)

configure_file(
    ${CMAKE_SOURCE_DIR}/config.h.cmake
    ${CMAKE_SOURCE_DIR}/config.h
)

add_subdirectory(src)
