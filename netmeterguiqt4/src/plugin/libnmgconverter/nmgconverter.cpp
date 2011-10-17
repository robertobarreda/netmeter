/***************************************************************************
 *   Copyright (C) 2008 by Joan Sala Calero <jsala@ac.upc.edu>             *
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

#include "nmgconverter.h"
#include <nmgmodulemanager.h>

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @return
 */
NMGConverter::NMGConverter()
{
	moduleName = MOD_NAME;
	moduleCapabilities = MOD_CAPABILITIES;
	moduleVersion = MOD_VERSION;
	moduleGUIName = GUI_NAME;
	widget = 0;
}

/**
 * Destructor of the class
 * @return
 */
NMGConverter::~NMGConverter()
{
	if (widget) delete widget;
}

/**
 * initModuleGUI
 * @return
 */
void NMGConverter::initModuleGUI()
{
}

/**
 * removeModuleGUI
 * @return
 */
void NMGConverter::removeModuleGUI()
{
}

/**
 * createWidget
 * @return
 */
void NMGConverter::createWidget()
{
        if ( !widget ) widget = new NMGConverterWidget( this, this, 0, "File Converter" , &getPreferencesManager() );
}

/**
 * Returns the widget associated to the module
 * @return
 */
QWidget * NMGConverter::getWidget()
{
	createWidget();
	return widget;
}

/**
 * Returns the popup menu associated to the module
 * @return
 */
QMenu * NMGConverter::getPopupMenu()
{
	createWidget();
	return widget->getPopupMenu();
}

/**
 * Returns the toolbar associated to the module
 * @return
 */
QToolBar * NMGConverter::getToolBar()
{
	createWidget();
	return widget->getToolBar();
}

bool NMGConverter::setExecutionData( QString data )
{
	cout << "Data Recieved: " << qPrintable( data ) << endl;
	if ( data.contains("subscribe", Qt::CaseInsensitive) ) // Attend a SUBSCRIBE command
	{
		/*! \par Received Message Format for a SUBSCRIPTION. It's always empty.
		 * Waits until arrives a START command.
		 * <data>	
		 *  	<subscribe /> --> IT SHOULD BE ALWAYS AN EMPTY TAG. INFORMATION IS SENT 
                 *                        BY sender->sendCommand ( Start, "<data><test>" + fileResultPath + "</test></data>" );
		 * </data>
		*/
		return true;
	}
        return false;
}

void NMGConverter::commandReceived ( QString moduleName, NMGCommandType command, const QString & data )
{
	cout << "NMGGraphics::commandReceived (" << qPrintable ( NMGUtil::getCommandTypeName(command) );
        cout << ") from " << qPrintable ( moduleName ) << endl;

// 	if ( command == Start ) widget->loadData( data );
//      else cerr << "[ERROR] NMGConverter Command not supported" << endl;
}