# - Try to find pcap
# Once done this will define
#
#  PCAP_FOUND        - system has pcap
#  PCAP_INCLUDE_DIR  - the pcap include directory
#  PCAP_LIBRARIES    - Link these to use pcap


FIND_PATH(PCAP_INCLUDE_DIR pcap.h
  /usr/include
  /usr/local/include
  /opt/include
)

FIND_LIBRARY(PCAP_LIBRARY
  NAMES pcap
  PATHS /usr/lib
        /usr/local/lib
	/opt/lib
)

IF (PCAP_LIBRARY)
  SET( PCAP_FOUND "YES" )
  SET( PCAP_LIBRARIES ${PCAP_LIBRARY} )
  MESSAGE(STATUS "Looking for Pcap library and headers - found")
ELSE (PCAP_LIBRARY)
  MESSAGE(STATUS "Looking for Pcap library and headers - not found")
ENDIF (PCAP_LIBRARY)

MARK_AS_ADVANCED(
  PCAP_LIBRARY
  PCAP_LIBRARIES
  PCAP_INCLUDE_DIR
)
