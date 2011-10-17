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
#include "nmgexemple.h"
#include <nmghostinformationmanager.h>

#include <iostream>
using namespace std;

/**
 * Constructor of the class
 * @return
 */
NMGExemple::NMGExemple()
{
	moduleName = MOD_NAME;
	moduleCapabilities = MOD_CAPABILITIES;
	moduleVersion = MOD_VERSION;
	moduleCoreMatching = CORE_MATCHING;
	moduleGUIName = GUI_NAME;
	widget = 0;

	addFieldToFieldsManager( "NMGExemple_PathX", "App X Path", "/path/x" );
	addFieldToFieldsManager( "NMGExemple_PathY", "App Y Path", "/path/y" );
}

/**
 * Destructor of the class
 * @return
 */
NMGExemple::~NMGExemple()
{}

/**
 * Returns the widget of the plugin
 * @return
 */
QWidget * NMGExemple::getWidget()
{
	if ( !widget ) widget = new NMGExempleWidget( this );
	return widget;
}

/**
 * Executed when the host widget is hidden
 * @param  station Station created
 */
void NMGExemple::notifyNewStation( NMGStation * station )
{
	cout << "NMExemple ..." << endl;
	if ( !station ) return;
}

void NMGExemple::dataReceived( QString data )
{
	cout << "**************************************" << endl;
	cout << "NMExemple receive start reply: " << endl << qPrintable ( data ) << endl;
	cout << "**************************************" << endl;
}

/**
 * Returns the pop up menu of the module
 * @return
 */
QMenu * NMGExemple::getPopupMenu()
{
	if ( !widget ) widget = new NMGExempleWidget( this );
	return widget->getPopupMenu();
}

/**
 * Initializes the plugin
 */
void NMGExemple::initModuleGUI()
{}

/**
 * Frees the plugin resources
 */
void NMGExemple::removeModuleGUI()
{}

