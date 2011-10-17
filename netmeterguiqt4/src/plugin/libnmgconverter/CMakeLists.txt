add_subdirectory(scripts)
add_subdirectory(xml)

set(NETMETERGUI_PLUGINS_CONVERTER ${NETMETERGUI_PLUGINS_DIR}/libnmgconverter)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_WIDGET_STATUSBAR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_PLUGINS_DIR})

set(libnmgconverter_UIC nmgconverterwidgetbase.ui)

set(libnmgconverter_SRCS nmgconverter.cpp
                         nmgconverterwidget.cpp
			 nmgconverterwidgetfromconf.cpp )

qt4_wrap_ui(libnmgconverter_UIC_H ${libnmgconverter_UIC})
qt4_automoc(${libnmgconverter_SRCS})

add_library(nmgconverter SHARED ${libnmgconverter_SRCS} ${libnmgconverter_UIC_H})

install(
    TARGETS nmgconverter
    LIBRARY DESTINATION ${PLUGIN_DIR}
)

