add_subdirectory(xml)
set(NETMETERGUI_PLUGINS_STATISTIC ${NETMETERGUI_PLUGINS_DIR}/libnmgstatistic)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_WIDGET_STATUSBAR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_PLUGINS_DIR})

set(libnmgstatistic_UIC nmgstatisticwidgetbase.ui
		        nmgstatisticexportoptionswidget.ui )

set(libnmgstatistic_SRCS nmgstatistic.cpp
			 nmgstatisticdata.cpp
			 nmgstatisticerror.cpp
			 nmgstatisticexportmanager.cpp
			 nmgstatisticfileloader.cpp
			 nmgstatisticparser.cpp
			 nmgstatisticremotewidget.cpp
			 nmgstatisticthread.cpp
			 nmgstatisticwidget.cpp )

qt4_wrap_ui(libnmgstatistic_UIC_H ${libnmgstatistic_UIC})
qt4_automoc(${libnmgstatistic_SRCS})

add_library(nmgstatistic SHARED ${libnmgstatistic_SRCS} ${libnmgstatistic_UIC_H})

install(
    TARGETS nmgstatistic
    LIBRARY DESTINATION ${PLUGIN_DIR}
)
