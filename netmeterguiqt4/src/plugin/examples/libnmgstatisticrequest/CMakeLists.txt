set(NETMETERGUI_PLUGINS_STATISTICREQUEST ${NETMETERGUI_PLUGINS_DIR}/libnmgstatisticrequest)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_WIDGET_STATUSBAR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_PLUGINS_DIR})

set(libnmgstatisticrequest_UIC 	nmgstatisticrequestwidgetbase.ui )
set(libnmgstatisticrequest_SRCS nmgstatisticrequest.cpp 
				nmgstatisticrequestwidget.cpp )

qt4_wrap_ui(libnmgstatisticrequest_UIC_H ${libnmgstatisticrequest_UIC})
qt4_automoc(${libnmgstatisticrequest_SRCS})

add_library(nmgstatisticrequest SHARED ${libnmgstatisticrequest_SRCS} 
					${libnmgstatisticrequest_UIC_H})
install(
    TARGETS nmgstatisticrequest 
    LIBRARY DESTINATION ${PLUGIN_DIR}
)

