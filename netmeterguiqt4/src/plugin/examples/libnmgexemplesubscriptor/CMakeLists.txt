set(NETMETERGUI_PLUGINS_EXEMPLEREQUEST ${NETMETERGUI_PLUGINS_DIR}/libnmgexemplesubscripted)
set(NETMETERGUI_PLUGINS_EXEMPLEADDER ${NETMETERGUI_PLUGINS_DIR}/libnmgexemplesubscriptor)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_WIDGET_STATUSBAR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_PLUGINS_DIR})

set(libnmgexemplesubscripted_UIC 	nmgexemplesubscriptedwidgetbase.ui )
set(libnmgexemplesubscripted_SRCS 	nmgexemplesubscripted.cpp 
					nmgexemplesubscriptedwidget.cpp )

set(libnmgexemplesubscriptor_UIC 	nmgexemplesubscriptorwidgetbase.ui )
set(libnmgexemplesubscriptor_SRCS 	nmgexemplesubscriptor.cpp 
					nmgexemplesubscriptorwidget.cpp)

qt4_wrap_ui(libnmgexemplesubscripted_UIC_H ${libnmgexemplesubscripted_UIC})
qt4_automoc(${libnmgexemplesubscripted_SRCS})

qt4_wrap_ui(libnmgexemplesubscriptor_UIC_H ${libnmgexemplesubscriptor_UIC})
qt4_automoc(${libnmgexemplesubscriptor_SRCS})

add_library(nmgexemplesubscripted SHARED ${libnmgexemplesubscripted_SRCS} ${libnmgexemplesubscripted_UIC_H})
add_library(nmgexemplesubscriptor SHARED ${libnmgexemplesubscriptor_SRCS} ${libnmgexemplesubscriptor_UIC_H})

install(
    TARGETS nmgexemplesubscripted nmgexemplesubscriptor
    LIBRARY DESTINATION ${PLUGIN_DIR}
)

