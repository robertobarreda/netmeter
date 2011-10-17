include_directories(${LIB_NETMETER_CORE})

set(lib_SRCS nmbuffer.cpp
             nmdatetime.cpp
	     nmdns.cpp
	     nmentity.cpp
	     nmerror.cpp
	     nmevent.cpp
	     nmeventmanager.cpp
	     nmexternal.cpp
	     nmexternallocal.cpp
	     nmexternalremote.cpp
	     nmfile.cpp
	     nmftp.cpp
	     nmhost.cpp
	     nminterface.cpp
	     nminterpret.cpp
	     nmlist.cpp
	     nmmodule.cpp
	     nmmodulefactory.cpp
	     nmmodulemanager.cpp
	     nmmoduleinterface.cpp
	     nmmutex.cpp
	     nmparent.cpp
 	     nmpython.cpp
	     nmscp.cpp
	     nmserversocket.cpp
	     nmsettings.cpp
	     nmsocket.cpp
	     nmsocketremote.cpp
	     nmstation.cpp
	     nmstatus.cpp
	     nmstring.cpp
	     nmtask.cpp
	     nmtaskmanager.cpp
	     nmtaskclean.cpp
	     nmtaskdirty.cpp
	     nmtaskstatus.cpp
	     nmtaskcompleted.cpp
	     nmtaskstop.cpp
             nmtaskpurgeplugin.cpp
	     nmtaskquit.cpp
	     nmtaskstat.cpp
	     nmtaskdeattach.cpp
	     nmtaskconfigure.cpp
	     nmtaskstart.cpp
	     nmthread.cpp
             nmtimer.cpp
	     nmxml.cpp
	     nmxmlstreamwriter.cpp
)

include_directories(${CMAKE_SOURCE_DIR} ${NETMETER_CORE_DIR})

add_library(netmeter SHARED ${lib_SRCS})
target_link_libraries(netmeter ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${PYTHON_LIBRARIES})

set(libnetmeter_include_HEADERS
       nmbuffer.h
       nmdatetime.h
       nmdns.h
       nmentity.h
       nmerror.h
       nmevent.h
       nmeventmanager.h
       nmexternal.h
       nmexternallocal.h
       nmexternalremote.h
       nmfile.h
       nmftp.h
       nmhost.h
       nminterface.h
       nminterpret.h
       nmlist.h
       nmmodulefactory.h
       nmmodule.h
       nmmoduleinterface.h
       nmmodulemanager.h
       nmmutex.h
       nmparent.h
       nmpython.h
       nmscp.h
       nmserversocket.h
       nmsettings.h
       nmsocket.h
       nmsocketremote.h
       nmstation.h
       nmstatus.h
       nmstring.h
       nmtaskclean.h
       nmtaskcompleted.h
       nmtaskconfigure.h
       nmtaskdeattach.h
       nmtaskdirty.h
       nmtask.h
       nmtaskmanager.h
       nmtaskpurgeplugin.h
       nmtaskquit.h
       nmtaskstart.h
       nmtaskstat.h
       nmtaskstatus.h
       nmtaskstop.h
       nmthread.h
       nmtimer.h
       nmxml.h
       nmxmlstreamwriter.h
)

install(
    TARGETS netmeter
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

install(
        FILES ${libnetmeter_include_HEADERS}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netmeter
)
