set(NETMETERGUI_PLUGINS_TRACE ${NETMETERGUI_PLUGINS_DIR}/libnmgtrace)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_WIDGET_STATUSBAR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_PLUGINS_DIR})

set(libnmgtrace_UIC nmgtracewidgetbase.ui
)
set(libnmgtrace_SRCS nmgtrace.cpp
                     nmgtracewidget.cpp
		     hostlistviewitem.cpp
)

qt4_wrap_ui(libnmgtrace_UIC_H ${libnmgtrace_UIC})
qt4_automoc(${libnmgtrace_SRCS})

add_library(nmgtrace SHARED ${libnmgtrace_SRCS} ${libnmgtrace_UIC_H})

install(
    TARGETS nmgtrace
    LIBRARY DESTINATION ${PLUGIN_DIR}
)

