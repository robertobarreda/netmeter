# - Try to find libnetmeter
# Once done this will define
#
#  LIBNETMETER_FOUND        - system has libnetmeter
#  LIBNETMETER_INCLUDE_DIR  - the libnetmeter include directory
#  LIBNETMETER_LIBRARIES    - Link these to use libnetmeter
#  LIBNETMETER_DEFINITIONS  - Compile switches

FIND_PATH(LIBNETMETER_INCLUDE_DIR nmparent.h
  ${LIBNETMETER_BASE}/include/netmeter
  /usr/include
  /usr/include/netmeter
  /usr/local/include
  /usr/local/include/netmeter
  /opt/include
  /opt/include/netmeter
)

FIND_LIBRARY(LIBNETMETER_LIBRARY
  NAMES netmeter
  PATHS ${LIBNETMETER_BASE}/lib
        /usr/lib
        /usr/local/lib
        /opt/lib
)

IF (LIBNETMETER_LIBRARY)
  SET( LIBNETMETER_FOUND "YES" )
  SET( LIBNETMETER_LIBRARIES ${LIBNETMETER_LIBRARY} )
  MESSAGE(STATUS "Looking for libnetmeter library and headers - found")
ELSE (LIBNETMETER_LIBRARY)
  MESSAGE(STATUS "Looking for libnetmeter library and headers - not found")
ENDIF (LIBNETMETER_LIBRARY)

MARK_AS_ADVANCED(
  LIBNETMETER_LIBRARY
  LIBNETMETER_LIBRARIES
  LIBNETMETER_INCLUDE_DIR
)

