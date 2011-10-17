include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_PLUGINS_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_GUI_DIR})

set(libpopupmessage_SRCS overlaywidget.cpp
                         popupmessage.cpp
			 messagebar.cpp
			 overlaywidget.h
                         popupmessage.h
			 messagebar.h
)

qt4_automoc(${libpopupmessage_SRCS})

add_library(popupmessage SHARED ${libpopupmessage_SRCS})

install(
    TARGETS popupmessage
    LIBRARY DESTINATION ${NETMETER_LIB_DIR}
)

