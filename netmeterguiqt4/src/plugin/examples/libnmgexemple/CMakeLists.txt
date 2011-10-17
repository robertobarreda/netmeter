set(NETMETERGUI_PLUGINS_TRACE ${NETMETERGUI_PLUGINS_DIR}/libnmgexemple)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_WIDGET_STATUSBAR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_PLUGINS_DIR})

set(libnmgexemple_SRCS nmgexemple.cpp nmgexemplewidget.cpp)

qt4_automoc(${libnmgexemple_SRCS})

add_library(nmgexemple SHARED ${libnmgexemple_SRCS})

install(
    TARGETS nmgexemple
    LIBRARY DESTINATION ${PLUGIN_DIR}
)

