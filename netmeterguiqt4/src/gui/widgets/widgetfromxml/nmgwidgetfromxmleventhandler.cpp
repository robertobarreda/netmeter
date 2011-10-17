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
#include "nmgwidgetfromxmleventhandler.h"

#include <QList>

#include <iostream>
using namespace std;

/**
 * Constructor of the class
 * @param parent QT's parent parameter
 * @param name QT's name parameter
 * @return
 */
NMGWidgetFromXMLEventHandler::NMGWidgetFromXMLEventHandler( QObject *parent, const char *name )
		: QObject( parent )
{
	setObjectName( name );
}


/**
 * Destructor of the class
 * @return
 */
NMGWidgetFromXMLEventHandler::~NMGWidgetFromXMLEventHandler()
{
	QList<NMGWidgetFromXMLAction *> l = actionMap.values();
	for ( QList<NMGWidgetFromXMLAction *>::iterator it = l.begin(); it != l.end(); )
	{
		NMGWidgetFromXMLAction * a = *it;
		delete a;
		++it;
	}
}


/**
 * Adds a action to the event handler
 * @param id Id of the button
 * @param a Action
 */
void NMGWidgetFromXMLEventHandler::addAction( QString id, NMGWidgetFromXMLAction  * a )
{
	ButtonIterator it = buttonMap.find( id );
	if ( it == buttonMap.end() ) cerr << "There is no button with id " << qPrintable( id ) << "." << endl;
	else
	{
		//UNUSED
		//a->setInputData(this);
		NMGWidgetFromXMLButton * b = it.value();
		b->connect( b, SIGNAL( buttonClicked( QString ) ), this, SLOT( actionButton( QString ) ) );
		actionMap.insert( id, a );
	}

}

/**
 * Adds a button to the event handler with identifier id
 * @param id identifier of the button to ad
 * @param b button to add
 */
void NMGWidgetFromXMLEventHandler::addButton( QString id, NMGWidgetFromXMLButton * b )
{
	buttonMap.insert( id, b );
}

/**
 * Executes hte action of the button with id buttonId
 * @param buttonId
 */
void NMGWidgetFromXMLEventHandler::actionButton( QString buttonId )
{
	ActionIterator it = actionMap.find( buttonId );
	if ( it == actionMap.end() )
		cerr << "There was no action for the button with id " << qPrintable( buttonId ) << "." << endl;
	else
	{
		it.value()->execute();
	}
}

/**
 * Returns the button associated to the id
 * @param id id of the button
 * @return Button or 0 if it doesn't exist
 */
NMGWidgetFromXMLButton * NMGWidgetFromXMLEventHandler::getButton( QString id )
{
	ButtonIterator it = buttonMap.find( id );
	if ( it == buttonMap.end() ) return 0;
	else return it.value();
}


#include "nmgwidgetfromxmleventhandler.moc"
