set(NETMETERGUI_PLUGINS_SIMULATOR ${NETMETERGUI_PLUGINS_DIR}/libnmgsimulator)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_WIDGET_STATUSBAR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_PLUGINS_DIR})

#set(QT_USE_QT3SUPPORT TRUE)

set(libnmgsimulator_UIC nmgsimulatorwidgetbase.ui)
set(libnmgsimulator_SRCS nmgsimulator.cpp
                   nmgsimulatorwidget.cpp
)

qt4_wrap_ui(libnmgsimulator_UIC_H ${libnmgsimulator_UIC})
qt4_automoc(${libnmgsimulator_SRCS})

add_library(nmgsimulator SHARED ${libnmgsimulator_SRCS} ${libnmgsimulator_UIC_H})

install(
    TARGETS nmgsimulator
    LIBRARY DESTINATION ${PLUGIN_DIR}
)

