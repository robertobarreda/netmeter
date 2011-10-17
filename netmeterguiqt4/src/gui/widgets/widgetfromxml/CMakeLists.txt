include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_PLUGINS_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_GUI_DIR})

set(libwidgetfromxml_SRCS nmgwidgetfromxml.cpp
                          nmgwidgetfromxmlbutton.cpp
			  nmgwidgetfromxmltextline.cpp
			  nmgwidgetfromxmleventhandler.cpp
			  nmgwidgetfromxmlaction.cpp
			  nmgwidgetfromxmlbuttongroup.cpp
			  nmgwidgetfromxmlvariable.cpp
)

qt4_automoc(${libwidgetfromxml_SRCS})

add_library(widgetfromxml SHARED ${libwidgetfromxml_SRCS})

install(
    TARGETS widgetfromxml
    LIBRARY DESTINATION ${NETMETER_LIB_DIR}
)

