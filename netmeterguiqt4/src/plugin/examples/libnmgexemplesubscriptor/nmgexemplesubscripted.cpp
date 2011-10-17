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
#include "nmgexemplesubscripted.h"

/**
 * Contructor of the class
 * @return
 */
NMGExempleSubscripted::NMGExempleSubscripted()
		: NMGModule()
{
	moduleName = "NMGExempleSubscripted";
	moduleCapabilities = "example";
	moduleVersion = "1.0";
	moduleCoreMatching = "NMExempleSubscripted";
	moduleGUIName = "Subscripted";
	widget = 0;
}

/**
 * Destructor of the class
 * @return
 */
NMGExempleSubscripted::~NMGExempleSubscripted()
{}


/**
 * Receives data of a start command
 * @param s
 * @return
 */
void NMGExempleSubscripted::dataReceived ( QString s )
{
	cout << "NMGExempleSubscripted::dataReceived" << endl << qPrintable( s ) << endl;
}

/**
 * Returns the widget of the module
 * @return
 */
QWidget * NMGExempleSubscripted::getWidget()
{
	if ( !widget ) widget = new NMGExempleSubscriptedWidget ( this, this );
	return widget;
}

/**
 * Sets the execution data
 * @param data
 * @return
 */
bool NMGExempleSubscripted::setExecutionData ( QString data )
{
	cout << "NMGExempleSubscripted::setExecutionData" << endl << qPrintable( data ) << endl;
	return true;
}

/**
 * Executed whan an error is received form the server
 * @param s
 */
void NMGExempleSubscripted::errorReceived ( QString s )
{
	cout << "NMGExempleSubscripted::errorReceived" << endl << qPrintable( s ) << endl;
}

/**
 * The result of a status to the core module is received
 * @param s
 */
void NMGExempleSubscripted::statusReceived ( QString s )
{
	cout << "NMGExempleSubscripted::statusReceived" << endl << qPrintable( s ) << endl;
}
