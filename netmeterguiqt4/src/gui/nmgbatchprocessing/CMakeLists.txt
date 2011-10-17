include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_PLUGINS_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_GUI_DIR})

set(libnmgbatchprocessing_UIC nmgbatchprocesswidgetbase.ui
)

set(libnmgbatchprocessing_SRCS nmgbatchprocesswidget.cpp
                               nmgbatchtask.cpp
			       nmgbatchtask.h
)

qt_add_ui_files(libnmgbatchprocessing_SRCS ${libnmgbatchprocessing_UIC})
qt4_automoc(${libnmgbatchprocessing_SRCS})
add_library(nmgbatchprocessing SHARED ${libnmgbatchprocessing_SRCS})

install(
    TARGETS nmgbatchprocessing
    LIBRARY DESTINATION ${NETMETER_LIB_DIR}
)

