set(NETMETERGUI_PLUGINS_NETPERF ${NETMETERGUI_PLUGINS_DIR}/libnmgnetperf)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_WIDGET_STATUSBAR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_PLUGINS_DIR})

set(libnmgnetperf_UIC nmgnetperfwidgetbase.ui
)

set(libnmgnetperf_SRCS nmgnetperf.cpp
                       nmgnetperfwidget.cpp
                       nmgnetperfwidgeterrornotifier.cpp
                       nmgnetperfxmlinterpret.cpp
)

qt4_wrap_ui(libnmgnetperf_UIC_H ${libnmgnetperf_UIC})
qt4_automoc(${libnmgnetperf_SRCS})

add_library(nmgnetperf SHARED ${libnmgnetperf_SRCS} ${libnmgnetperf_UIC_H})

install(
    TARGETS nmgnetperf
    LIBRARY DESTINATION ${PLUGIN_DIR}
)

