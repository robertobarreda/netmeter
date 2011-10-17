include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_PLUGINS_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_WIDGET_STATUSBAR})

add_subdirectory(widgets)
add_subdirectory(plugin)
#add_subdirectory(nmgbatchprocessing)

set(libgui_SRCS nmgapplicationinformation.cpp
                nmgconnection.cpp
		nmginterpret.cpp
		nmgrandomgenerator.cpp
		nmgmessagecenter.cpp
		nmgresulthandler.cpp
		nmggzipfile.cpp
		nmghostinformationmanager.cpp
		nmghostinformation.cpp
		nmgmodulefieldsmanager.cpp
		nmgmodulefieldslist.cpp
		nmgconfigurationfilemanager.cpp
		nmgxmlconfigurationfile.cpp
		nmgconfigurationfilefactory.cpp
		nmgplaintextconfigurationfile.cpp
		nmgstation.cpp
		nmghost.cpp
		nmgstationfieldsmanager.cpp
		nmgstationfield.cpp
		nmgpreferencesconfigurationfile.cpp
		nmgpreferencesmanager.cpp
		nmgpreferencesmanager.h
		nmgimagemanager.cpp
		nmgdnsresolver.cpp
		nmgmd5.cpp
)

qt4_automoc(${libgui_SRCS})
add_library(netmetergui SHARED ${libgui_SRCS})
target_link_libraries(netmetergui nmgplugins nmgwidgets)

install(
    TARGETS netmetergui
    LIBRARY DESTINATION ${NETMETER_LIB_DIR}
)

