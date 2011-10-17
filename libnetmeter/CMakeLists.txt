PROJECT(libnetmeter)

cmake_minimum_required(VERSION 2.4.8)

if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeOptions.txt)
        include(${CMAKE_SOURCE_DIR}/CMakeOptions.txt)
else(EXISTS ${CMAKE_SOURCE_DIR}/CMakeOptions.txt)
        message(FATAL_ERROR "CMakeOptions.txt not found! Run configure.cmake first.")
endif(EXISTS ${CMAKE_SOURCE_DIR}/CMakeOptions.txt)

#if you don't want the full compiler output, remove the following line
#SET(CMAKE_VERBOSE_MAKEFILE ON)

find_package(LibXml2 REQUIRED) # find and setup libXML2 for this project
find_package(LibXslt REQUIRED) # find and setup libXML2 for this project
find_package(PythonLibs REQUIRED) # find and setup python libs for this project

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-g -Wall -O2 ${LIBXSLT_DEFINITIONS} ${LIBXML2_DEFINITIONS} -DHAVE_CONFIG_H)

#need to link to some other libraries ? just add them here

set(LIB_NETMETER_CORE ${CMAKE_SOURCE_DIR}/src)

include_directories(${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR} ${PYTHON_INCLUDE_PATH})

if(NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX /usr/local/netmeter)
endif(NOT CMAKE_INSTALL_PREFIX)

configure_file(
    ${CMAKE_SOURCE_DIR}/config.h.cmake
    ${CMAKE_SOURCE_DIR}/config.h
)

add_subdirectory(src)
