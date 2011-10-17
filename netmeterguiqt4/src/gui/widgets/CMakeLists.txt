include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_PLUGINS_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_WIDGET_STATUSBAR})

add_subdirectory(widgetfromxml)
add_subdirectory(statusbarprogress)
add_subdirectory(popupmessage)

set(libnmwidgets_UIC nmghostinformationmainwidget.ui
		     nmgpreferencesbasewidget.ui
		     nmghostinformationcombowidgetbase.ui
                     nmgaboutwidget.ui
)

set(libnmwidgets_SRCS nmgtaskprogressbar.cpp
		      nmgtaskwidget.cpp
		      nmgmainwindow.cpp
		      nmgmaintabbar.cpp
		      nmgmaintabwidget.cpp
		      nmghostinformationwidget.cpp
		      nmghostinformationcombowidget.cpp
		      nmgmoduletoolbox.cpp
		      nmgmoduletoolboxitem.cpp
		      nmgmainwindowtoolbar.cpp
		      nmgmainwindowmenubar.cpp
		      nmgpreferencesmodulewidget.cpp
		      nmgestablishingconnectionwidget.cpp
)

qt4_wrap_ui(libnmwidgets_UIC_H ${libnmwidgets_UIC})
qt4_automoc(${libnmwidgets_SRCS})

add_library(nmgwidgets SHARED ${libnmwidgets_SRCS} ${libnmwidgets_UIC_H})
target_link_libraries(nmgwidgets statusbarprogress popupmessage widgetfromxml)

install(
    TARGETS nmgwidgets
    LIBRARY DESTINATION ${NETMETER_LIB_DIR}
)

