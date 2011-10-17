/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
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
#include "nmgstatistic.h"
#include <nmgmodulemanager.h>

#include <iostream>
using namespace std;

NMGStatistic::NMGStatistic()
{
	moduleName = MOD_NAME;
	moduleCapabilities = MOD_CAPABILITIES;
	moduleVersion = MOD_VERSION;
	moduleGUIName = GUI_NAME;
	widget = 0;
	remoteWidget = 0;
}

NMGStatistic::~NMGStatistic()
{
	if (widget) delete widget;
	if (remoteWidget) delete remoteWidget;
}

void NMGStatistic::initModuleGUI()
{}

void NMGStatistic::removeModuleGUI()
{}

void NMGStatistic::createWidget()
{
	if ( !widget ) widget = new NMGStatisticWidget( &getPreferencesManager() );
}

QWidget * NMGStatistic::getWidget()
{
	createWidget();
	return widget;
}

QMenu * NMGStatistic::getPopupMenu()
{
	createWidget();
	return widget->getPopupMenu();
}

QToolBar * NMGStatistic::getToolBar()
{
	createWidget();
	return widget->getToolBar();
}

bool NMGStatistic::setExecutionData( QString data )
{
	cout << "Data Recieved: " << qPrintable( data ) << endl;
	if ( data.contains("subscribe", Qt::CaseInsensitive) ) // Attend a SUBSCRIBE command
	{
		/*! \par Received Message Format for a SUBSCRIPTION. It's always empty.
		 * Waits until arrives the command.
		 * <data>	
		 *  	<subscribe /> --> IT SHOULD BE ALWAYS AN EMPTY TAG. INFORMATION IS SENT 
                 *                        BY sender->sendCommand ( Start, "<data><test>" + fileResultPath + "</test></data>" );
		 * </data>
		*/
		return true;
	}
	else // Attend a REQUEST command that have all the information in the same message
	{
		/*! \par Other kind of message recieved (REQUEST or ERROR). Need to be parsed.
		 * <data>	
		 *  	<request>
		 *  	 ...
		 *  	</request>
		 * </data>
                 */
		remoteWidget = new NMGStatisticRemoteWidget( NMGModuleManager::self(), getTestId() );
		return remoteWidget->loadData( data );
	}
}

void NMGStatistic::commandReceived ( QString moduleName, NMGCommandType command, const QString & data )
{
	cout << "NMGStatistic::commandReceived (" << qPrintable ( NMGUtil::getCommandTypeName(command) );
        cout << ") from " << qPrintable ( moduleName ) << endl;

	if ( command == Start ) widget->loadData( data );
        else cerr << "[ERROR] NMGStatistic Command not supported" << endl;
}
