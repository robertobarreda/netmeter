set(NETMETERGUI_PLUGINS_EXEMPLEREQUEST ${NETMETERGUI_PLUGINS_DIR}/libnmgexemplerequest)
set(NETMETERGUI_PLUGINS_EXEMPLEADDER ${NETMETERGUI_PLUGINS_DIR}/libnmgexempleadder)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_WIDGET_STATUSBAR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_PLUGINS_DIR})

set(libnmgexemplerequest_UIC 	nmgexemplerequestwidgetbase.ui )
set(libnmgexemplerequest_SRCS 	nmgexemplerequest.cpp 
				nmgexemplerequestwidget.cpp )

set(libnmgexempleadder_UIC 	nmgexempleadderwidgetbase.ui )
set(libnmgexempleadder_SRCS 	nmgexempleadder.cpp 
				nmgexempleadderwidget.cpp )

qt4_wrap_ui(libnmgexemplerequest_UIC_H ${libnmgexemplerequest_UIC})
qt4_automoc(${libnmgexemplerequest_SRCS})

qt4_wrap_ui(libnmgexempleadder_UIC_H ${libnmgexempleadder_UIC})
qt4_automoc(${libnmgexempleadder_SRCS})

add_library(nmgexemplerequest SHARED ${libnmgexemplerequest_SRCS} ${libnmgexemplerequest_UIC_H})
add_library(nmgexempleadder SHARED ${libnmgexempleadder_SRCS} ${libnmgexempleadder_UIC_H})

install(
    TARGETS nmgexemplerequest nmgexempleadder
    LIBRARY DESTINATION ${PLUGIN_DIR}
)

