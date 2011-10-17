set(NETMETERGUI_PLUGINS_GRAPH ${NETMETERGUI_PLUGINS_DIR}/libnmggraph)

include_directories(${CMAKE_SOURCE_DIR} ${LIBNETMETER_INCLUDE_DIR} ${NETMETERGUI_GUI_WIDGET_DIR} ${NETMETERGUI_GUI_DIR} ${NETMETERGUI_GUI_WIDGET_XML_DIR} ${NETMETERGUI_WIDGET_STATUSBAR} ${NETMETERGUI_WIDGET_POPUP} ${NETMETERGUI_PLUGINS_DIR})

set(libnmggraph_UIC nmggraphicswidgetbase.ui)

set(libnmggraph_SRCS nmgglobals.cpp
                     nmggraphics.cpp 
                     nmggraphicswidget.cpp
                     nmgtoolbar.cpp
                     nmggraphicserror.cpp
                     nmgmeasurelistwidget.cpp
                     nmgplotwidget.cpp
                     nmgchartscene.cpp
                     nmgchartitem.cpp
                     nmgobjectinspector.cpp
                     nmgpropertyeditor.cpp
                     nmgpropertyeditordelegate.cpp

                     nmgflowdata.cpp
                     nmgfloworientedresultdata.cpp
                     nmgaggregationorientedresultdata.cpp
                     nmgtestfiledata.cpp
                     nmgfilecontainer.cpp
                     nmggraphpacketorienteddataparser.cpp
                     nmggraphaggregationorienteddataparser.cpp
                     nmgtestloaderhandler.cpp
                     nmgtestreaderthread.cpp
)

qt4_wrap_ui(libnmggraph_UIC_H ${libnmggraph_UIC})
qt4_automoc(${libnmggraph_SRCS})

add_library(nmggraph SHARED ${libnmggraph_SRCS} ${libnmggraph_UIC_H})

install(
    TARGETS nmggraph
    LIBRARY DESTINATION ${PLUGIN_DIR}
)