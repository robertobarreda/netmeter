include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR})

set(libnmgplugins_SRCS nmgmodule.cpp
                      nmgmoduleregister.cpp
		      nmgmodulemanager.cpp
		      nmgmoduleinstancemanager.cpp
		      nmgmoduleparent.cpp
		      nmwidgetfactory.cpp
		      nmgmodulefactory.cpp
		      nmgmodulecommunicationmanager.cpp
		      nmgmodulesubscriptionlist.cpp
		      nmgmoduleutils.cpp
		      nmgmodtabwidgetmanager.cpp
		      nmgmodulepreferencesmanager.cpp
		      nmgmoduleutils.h
		      nmgmetadata.cpp
		      nmgmetadataparser.cpp
		      nmgpacketorienteddataparser.cpp
		      nmgaggregationorienteddataparser.cpp
		      nmggrouptreewidget.cpp
		      nmggrouptreewidgetitem.cpp
)

qt4_automoc(${libnmgplugins_SRCS})
add_library(nmgplugins SHARED ${libnmgplugins_SRCS})

install(
    TARGETS nmgplugins
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

