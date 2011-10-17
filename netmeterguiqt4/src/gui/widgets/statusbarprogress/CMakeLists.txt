include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_PLUGINS_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_GUI_DIR})

set(libstatusbarprogress_SRCS overlaywidget.cpp
                              progressdialog.cpp
			      progressmanager.cpp
			      statusbarprogresswidget.cpp
			      overlaywidget.h
			      progressmanager.h
			      statusbarprogresswidget.h
)

qt4_automoc(${libstatusbarprogress_SRCS})

add_library(statusbarprogress SHARED ${libstatusbarprogress_SRCS})

install(
    TARGETS statusbarprogress
    LIBRARY DESTINATION ${NETMETER_LIB_DIR}
)

