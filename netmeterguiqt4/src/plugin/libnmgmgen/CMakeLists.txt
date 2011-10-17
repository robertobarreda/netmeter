add_subdirectory(xml)
set(NETMETERGUI_PLUGINS_MGEN ${NETMETERGUI_PLUGINS_DIR}/libnmgmgen)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_WIDGET_STATUSBAR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_PLUGINS_DIR})

#set(QT_USE_QT3SUPPORT TRUE)

set(libnmgmgen_UIC nmgmgenmainwidget.ui)
set(libnmgmgen_SRCS nmgmgen.cpp
                   nmgmgenwidget.cpp
		   nmgmgenflow.cpp
		   nmgmgenflowmodification.cpp
		   nmgmgenflowlist.cpp
		   nmgmgenflowmanager.cpp
		   nmgmgenbandwith.cpp
		   nmgmgenflowlistitem.cpp
		   nmgmgenflowextensionheader.cpp
		   nmgmgenflowrsvp.cpp
		   nmgxmlflowsortedlist.cpp
		   nmgmgenxmlinterpret.cpp
		   nmgmgenwidgeterrornotifier.cpp
)

qt4_wrap_ui(libnmgmgen_UIC_H ${libnmgmgen_UIC})
qt4_automoc(${libnmgmgen_SRCS})

add_library(nmgmgen SHARED ${libnmgmgen_SRCS} ${libnmgmgen_UIC_H})
#set_target_properties(nmgmgen
#        PROPERTIES LOCATION ${PLUGIN_DIR}
#        INSTALL_RPATH ${NETMETER_LIB_DIR}
#        PREFIX ""
#)

install(
    TARGETS nmgmgen
    LIBRARY DESTINATION ${PLUGIN_DIR}
)

