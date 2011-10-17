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
#include "nmgmaintabbar.h"

#include <iostream>
using namespace std;


/**
 * Constructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGMainTabBar::NMGMainTabBar( QWidget *parent, const char *name )
		: QTabBar( parent )
{
	setObjectName( name );
	menu.addAction( "Close tab", this, SLOT( deleteCurrent() ) );
	menu.addAction( "Close all tabs", this, SLOT( deleteAll() ) );
	menu.addAction( "Close other tabs", this, SLOT( deleteOthers() ) );
}


/**
 * Destructor of the class
 * @return
 */
NMGMainTabBar::~NMGMainTabBar()
{}

/**
 * Slot executed when the user wants to close the current tab
 */
void NMGMainTabBar::deleteCurrent()
{
	emit deleteCurrentTab();
}

/**
 * Slot executed when the user wants to close all the tabs
 */
void NMGMainTabBar::deleteAll()
{
	emit deleteAllTabs();
}

/**
 * Slot executed when the user wanbts to close all the tabs but not the current
 */
void NMGMainTabBar::deleteOthers()
{
	emit deleteOthersTabs();
}

/**
 * Executed when the moue is pressed
 * @param e
 */
void NMGMainTabBar::mousePressEvent( QMouseEvent *e )
{
	if ( e->button() == Qt::RightButton )
	{
		//set the current tab
		QMouseEvent newEvent( QEvent::MouseButtonPress, e->pos(), e->globalPos(), Qt::LeftButton, e->buttons(), e->modifiers() );
		QTabBar::mousePressEvent( &newEvent );

		menu.popup( e->globalPos() );
	}
	else QTabBar::mousePressEvent( e );  /* per a que ho gestioni qui toqui */
}


#include "nmgmaintabbar.moc"
