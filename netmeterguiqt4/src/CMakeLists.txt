set(NETMETERGUI_GUI_DIR ${CMAKE_SOURCE_DIR}/src/gui)
set(NETMETERGUI_GUI_WIDGET_DIR ${CMAKE_SOURCE_DIR}/src/gui/widgets)
set(NETMETERGUI_GUI_WIDGET_XML_DIR ${CMAKE_SOURCE_DIR}/src/gui/widgets/widgetfromxml)
set(NETMETERGUI_PLUGINS_DIR ${CMAKE_SOURCE_DIR}/src/gui/plugin)
set(NETMETERGUI_WIDGET_POPUP ${CMAKE_SOURCE_DIR}/src/gui/widgets/popupmessage)
set(NETMETERGUI_WIDGET_STATUSBAR ${CMAKE_SOURCE_DIR}/src/gui/widgets/statusbarprogress)

set(LIB_NETMETER_GUI netmetergui)
set(LIB_NETMETER_PLUGINS nmgplugins)

#set(QT_USE_QT3SUPPORT TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTSVG TRUE)
include(${QT_USE_FILE})

add_subdirectory(gui)
add_subdirectory(conf)
add_subdirectory(plugin)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_PLUGINS_DIR})

ADD_EXECUTABLE(netmetergui_bin main.cpp nmgmain.cpp)
target_link_libraries(netmetergui_bin util ${LIBNETMETER_LIBRARY} ${LIB_NETMETER_GUI} ${LIB_NETMETER_PLUGINS} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${QT_LIBRARIES})

set_target_properties(netmetergui_bin PROPERTIES
  INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
  INSTALL_RPATH_USE_LINK_PATH true
  OUTPUT_NAME netmetergui
)

install(
    TARGETS netmetergui_bin
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
