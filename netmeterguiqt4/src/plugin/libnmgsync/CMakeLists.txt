set(NETMETERGUI_PLUGINS_SYNC ${NETMETERGUI_PLUGINS_DIR}/libnmgsync)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_WIDGET_STATUSBAR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_PLUGINS_DIR})

set(libnmgsync_UIC nmgsyncwidgetbase.ui
)

set(libnmgsync_SRCS nmgsync.cpp
                    nmgsyncwidget.cpp
		    nmgsyncremote.cpp
)

qt4_wrap_ui(libnmgsync_UIC_H ${libnmgsync_UIC})
qt4_automoc(${libnmgsync_SRCS})

add_library(nmgsync SHARED ${libnmgsync_SRCS} ${libnmgsync_UIC_H})

install(
    TARGETS nmgsync
    LIBRARY DESTINATION ${PLUGIN_DIR}
)

