/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
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
#include "nmgexemplesubscriptor.h"

#include <QListWidget>

#include <iostream>
using namespace std;


/**
 * Contructor of the class
 * @return
 */
NMGExempleSubscriptor::NMGExempleSubscriptor()
		: NMGModule()
{

	moduleName = "NMGExempleSubscriptor";
	moduleCapabilities = "example";
	moduleVersion = "1.0";
	moduleCoreMatching = "NMExempleSubscriptor";
	moduleGUIName = "Subscriptor";
	widget = 0;
}

/**
 * Destructor of the class
 * @return
 */
NMGExempleSubscriptor::~NMGExempleSubscriptor()
{}

/**
 * Receives data of a start command
 * @param s
 * @return
 */
void NMGExempleSubscriptor::dataReceived ( QString s )
{
	cout << "NMGExempleSubscriptor::dataReceived" << endl << qPrintable ( s ) << endl;
}

/**
 * Command received from a subscripted module
 * @param moduleName Name of the module subscripted
 * @param command Commnad executed by the module
 */
void NMGExempleSubscriptor::commandReceived ( QString moduleName, NMGCommandType command )
{
	cout << "NMGExempleSubscriptor::commandReceived" << endl;
	cout << "Command received from " << qPrintable ( moduleName );
	cout << ": " << qPrintable ( NMGUtil::getCommandTypeName(command) ) << endl;
	QString listItem = " * " + moduleName + "           " + commandString;
	widget->list->addItem ( listItem );
}

/**
 * Returns the widget of the module
 * @return
 */
QWidget * NMGExempleSubscriptor::getWidget()
{
	if ( !widget) widget = new NMGExempleSubscriptorWidget();
	return widget;
}

/**
 * Sets the execution data
 * @param data
 * @return
 */
bool NMGExempleSubscriptor::setExecutionData ( QString data )
{
	cout << "NMGExempleSubscriptor::setExecutionData" << endl << qPrintable ( data ) << endl;
	return true;
}

/**
 * Executed whan an error is received form the server
 * @param s
 */
void NMGExempleSubscriptor::errorReceived ( QString s )
{
	cout << "NMGExempleSubscriptor::errorReceived" << endl << qPrintable ( s ) << endl;
}

/**
 * The result of a status to the core module is received
 * @param s
 */
void NMGExempleSubscriptor::statusReceived ( QString s )
{
	cout << "NMGExempleSubscriptor::statusReceived" << endl << qPrintable ( s ) << endl;
}

