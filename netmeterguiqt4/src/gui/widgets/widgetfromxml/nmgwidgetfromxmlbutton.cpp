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
#include "nmgwidgetfromxmlbutton.h"

#include <iostream>
using namespace std;

/**
 * Constructor of the class
 * @param parent Qt's parent parameter
 * @param name Qt's name parameter
 * @return
 */
NMGWidgetFromXMLButton::NMGWidgetFromXMLButton( QWidget *parent, const char *name )
		: QPushButton( name, parent )
{
	connect( this, SIGNAL( clicked() ), this, SLOT( actionClicked() ) );
}

/**
 * Constructor of the class
 * @param text Text of the button
 * @param parent Qt's parent parameter
 * @param name Qt's name parameter
 * @return
 */
NMGWidgetFromXMLButton::NMGWidgetFromXMLButton( QString text, QWidget *parent, const char *name )
		: QPushButton( name, parent )
{
	setText( text );
	connect( this, SIGNAL( clicked() ), this, SLOT( actionClicked() ) );
}

/**
 * Destructor of the class
 * @return
 */
NMGWidgetFromXMLButton::~NMGWidgetFromXMLButton()
{
}

/**
 * Returns the parameter id
 * @return The id of the button
 */
QString NMGWidgetFromXMLButton::getId()
{
	return objectName();
}

/**
 * Action performed when the button is clicked
 */
void NMGWidgetFromXMLButton::actionClicked()
{
	emit buttonClicked( getId() );
}

#include "nmgwidgetfromxmlbutton.moc"
