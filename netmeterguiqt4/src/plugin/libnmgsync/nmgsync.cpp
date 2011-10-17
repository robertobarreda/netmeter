/***************************************************************************
 *   Copyright (C) 2005 by Joan Sala                                       *
 *   jsala@ac.upc.edu                                                      *
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

#include "nmgsync.h"
#include <nmghostinformationmanager.h>

#include <iostream>
using namespace std;

/**
 * Constructor of the class
 * @return 
 */
NMGSync::NMGSync()
{
	moduleName = MOD_NAME;
	moduleCapabilities = MOD_CAPABILITIES;
	moduleVersion = MOD_VERSION;
	moduleCoreMatching = CORE_MATCHING;
	moduleGUIName = GUI_NAME;
	widget = 0;
	remote = 0;
}

/**
 * Destructor of the class
 * @return 
 */
NMGSync::~NMGSync()
{
}

/**
 * Initializes the plugin
 */
void NMGSync::initModuleGUI()
{
}

/**
 * Frees the plugin resources
 */
void NMGSync::removeModuleGUI()
{
}

/**
 * Returns the widget of the plugin
 * @return 
 */
QWidget * NMGSync::getWidget()
{

	if(!widget) 	widget = new NMGSyncWidget(this);
	return widget;
}

/**
 * Executed when the host widget is hidden
 * @param  station Station created
 */
void NMGSync::notifyNewStation(NMGStation * station)
{
	cout << "NMGSync ..." << endl;
	if(!station) return;
}

void NMGSync::dataReceived(QString data)
{
	if(!widget) 	return;
	else		widget->resultReceived(data);
}

/**
 * Returns the pop up menu of the module
 * @return 
 */
QMenu * NMGSync::getPopupMenu()
{
	return 0;
}

void NMGSync::errorReceived(int errorType, QString errorMessage)
{
	#ifdef DEBUG
	cout << "NMGSync::errorReceived type = "<< errorType <<", message = "<< qPrintable(errorMessage) <<"."<<endl;
	#endif

	if(!widget) cerr << "[panic] SYNC Error received with no widget: type = "<< errorType <<", message = "<< qPrintable(errorMessage) <<endl;
	else 
	{
		cout << "**************************************"<< endl;
		cout << "NMSync receive ERROR reply: " << endl;
		cout << "type=" << errorType << ", message=" << qPrintable(errorMessage) << endl;
		cout << "**************************************"<< endl;
		widget->notifyError(errorType, errorMessage);
	}
}

void NMGSync::statusReceived(QString data)
{
}


/* NMGModuleComunication ::::::::: NMG* <-> NMGSync <-> NMSync */


bool NMGSync::setExecutionData(QString data)
{
	cout << "##########################################################" << endl;
	cout << "NMGSync::setExecutionData -> data: " << qPrintable( data ) << endl;
	cout << "##########################################################" << endl;
	
	conf = data;	// Store the test configuration

	remote = new NMGSyncRemote(this, getTestId());

	if(!remote)	return false;	// ERR
	else		return true;	// OK
}

void NMGSync::commandReceived(QString moduleName, NMGCommandType command, const QString &)
{
        QString commandString = NMGUtil::getCommandTypeName(command);
	cout << "##########################################################" << endl;
	cout << "NMGSync::commandRecieved:  " << qPrintable ( commandString );
        cout << " from " << qPrintable( moduleName ) << endl;
	cout << "##########################################################" << endl;

	if(!remote) 	remote = new NMGSyncRemote(this, getTestId());

	// Only two commands are possible:
	if(commandString == "Start")	remote->sendStart(conf);
	if(commandString == "Stop")	remote->sendStop();
}


