/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Ernest Nieto <ernest.nieto@gmail.com>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "nmgnetperf.h"

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @return
 */
NMGNetperf::NMGNetperf()
{
	moduleName = MOD_NAME;
	moduleCapabilities = MOD_CAPABILITIES;
	moduleVersion = MOD_VERSION;
	moduleCoreMatching = CORE_MATCHING;
	moduleGUIName = GUI_NAME;
	widget = 0;

	addFieldToFieldsManager ( ID_NETPERF_PATH, "NetPerf Path", "/usr/bin/netperf" );
	addFieldToFieldsManager ( ID_NETSERVER_PATH, "NetServer Path", "/usr/bin/netserver" );
}

/**
 * Returns the widget associated to the module
 * @return
 */
QWidget * NMGNetperf::getWidget()
{
	createWidget();
	return widget;
}

/**
 * Returns the popup menu associated to the module
 * @return
 */
QMenu * NMGNetperf::getPopupMenu()
{
	createWidget();
	return widget->getPopupMenu();
}

/**
 * Returns the toolbar associated to the module
 * @return
 */
QToolBar * NMGNetperf::getToolBar()
{
	createWidget();
	return widget->getToolBar();
}

/**
 * The server has sent  a result to a start request
 * @param data Data which was between the tags <data> ...</data> (including the tags)
 */
void NMGNetperf::dataReceived ( QString data )
{
	//TODO: test if pendent...
	if ( !widget ) cerr << "TODO: NMGNetperf::dataReceived (test if this is the case of a pendent message!)" << endl;
	else widget->notifyResult ( data );
}

/**
 * The server sent a error message to the module
 * @param errorType Type of the error (fatal = 1, user = 5, warning = 10, debug = 20)
 * @param errorMessage Error Message
 */
void NMGNetperf::errorReceived ( int errorType, QString errorMessage )
{
#ifdef DEBUG
	cout << "NMGNetperf::errorReceived type=" << errorType << ", message=" << qPrintable ( errorMessage ) << "." << endl;
#endif
	if ( !widget ) cerr << "[panic] NETPERF Error received with no widget: type=" << errorType << ", message=" << qPrintable ( errorMessage ) << endl;
	else widget->notifyError ( errorType, errorMessage );
}

/**
 * The server sent a status reply
 * @param data Data which was between the tags <data> ...</data> (including the tags)
 */
void NMGNetperf::statusReceived ( QString data )
{
	if ( !widget ) cerr << "[panic] NETPERF Status received with no widget:" << qPrintable ( data ) << endl;
	else widget->notifyStatus ( data );
}
