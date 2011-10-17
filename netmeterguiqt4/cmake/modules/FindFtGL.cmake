# - Try to find ftgl
# Once done this will define
#
#  FTGL_FOUND        - system has pcap
#  FTGL_INCLUDE_DIR  - the pcap include directory
#  FTGL_LIBRARIES    - Link these to use pcap
#  FTGL_DEFINITIONS  - Compile switches

IF (FTGL_INCLUDE_DIR AND FTGL_LIBRARIES)
   # in cache already
   SET(LibFTGL_FIND_QUIETLY TRUE)
ENDIF (FTGL_INCLUDE_DIR AND FTGL_LIBRARIES)

INCLUDE(UsePkgConfig)

PKGCONFIG(ftgl _FTGLIncDir _FTGLLinkDir _FTGLLinkFlags _FTGLCflags)
IF (_FTGLIncDir AND _FTGLLinkDir)
        SET(FTGL_DEFINITIONS ${_FTGLCflags})
        SET(FTGL_LIBRARIES ${_FTGLLinkFlags})
        SET(FTGL_INCLUDE_DIR ${_FTGLIncDir})
	SET(FTGL_FOUND TRUE)
        MESSAGE(STATUS "Found ftgl libraries: ${FTGL_LIBRARIES}")
        MESSAGE(STATUS "Found ftgl includes: ${FTGL_INCLUDE_DIR}")
ELSE (_FTGLIncDir AND _FTGLLinkDir)
        IF (FTGL_FIND_REQUIRED)
            MESSAGE(SEND_ERROR "Could not find ftgl")
        ELSE (FTGL_FIND_REQUIRED)
            MESSAGE(STATUS "Could not find ftgl")
        ENDIF (FTGL_FIND_REQUIRED)
ENDIF (_FTGLIncDir AND _FTGLLinkDir)

