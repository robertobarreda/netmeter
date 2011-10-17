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
#include "nmgmaintabwidget.h"
#include "nmgmaintabbar.h"
#include <nmgimagemanager.h>

#include <QScrollArea>
#include <QObject>
#include <QGridLayout>
#include <QPalette>

#include <iostream>
using namespace std;


/**
 * Contructor of the class
 * @param parent QT's parent parameter
 * @param name QT's name parameter
 * @return
 */
NMGMainTabWidget::NMGMainTabWidget( QWidget *parent, const char *name )
		: QTabWidget( parent )
{
	setObjectName( name );
	setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum ) );
	NMGMainTabBar * tabBar = new NMGMainTabBar( this, "tabBar" );
	connect( tabBar, SIGNAL( deleteCurrentTab() ), this, SLOT( deleteCurrentTab() ) );
	connect( tabBar, SIGNAL( deleteAllTabs() ), this, SLOT( deleteAllTabs() ) );
	connect( tabBar, SIGNAL( deleteOthersTabs() ), this, SLOT( deleteOtherTabs() ) );
	setTabBar( tabBar );
}

/**
 * Destructor of the class
 * @return
 */
NMGMainTabWidget::~NMGMainTabWidget()
{}

/**
 * Adds a tab to the tabWidget
 * @param child
 * @param label
 */
void NMGMainTabWidget::addTab( QWidget * child, const QString & label, const QString & icon )
{
	QScrollArea * scrollArea = new QScrollArea;
	scrollArea->setWidgetResizable( true );  
	scrollArea->setWidget( child );

	QPalette palette;
	palette.setColor( child->backgroundRole(), this->palette().color( this->backgroundRole() ));
	child->setPalette( palette );

	if ( !icon.isEmpty() )
		QTabWidget::addTab( scrollArea, QIcon( *NMGImageManager::loadPixmap( icon ) ), label );
	else
		QTabWidget::addTab( scrollArea, label );
	setCurrentWidget( scrollArea ); 
	scrollArea->show();
}

/**
 * Returns the currenmt page widget of a module
 * @return
 */
QWidget * NMGMainTabWidget::getCurrentPageWidget()
{
	QWidget * current = currentWidget();
	if ( !current ) return 0;

	if ( QString( current->metaObject()->className() ) == "QScrollArea" )
		if ((( QScrollArea * ) current )->widget() ) return (( QScrollArea * ) current )->widget();
	//cerr << "[panic] No Scroll View child at QTabWidget" << endl;
	return 0;
}

/**
 * Returns the widget of a module  at the i page
 * @param i Number of the page (first is 0)
 * @return
 */
QWidget * NMGMainTabWidget::getPageWidget( int i )
{
	QWidget * current = widget( i );
	if ( !current ) return 0;

	if ( QString( currentWidget()->metaObject()->className() ) == "QscrollArea" )
		if ((( QScrollArea * ) current )->widget() ) return (( QScrollArea * ) current )->widget();
	//cerr << "[panic] No Scroll View child at QTabWidget" << endl;
	return 0;
}

/**
 * Removes and deletes the current page widget
 */
void NMGMainTabWidget::deleteCurrentPage()
{
	removeTab(currentIndex ());
}

/**
 * Deletes and removes the page i
 * @param i Number of the page (first is 0)
 */
void NMGMainTabWidget::deletePage( int i )
{
	removeTab( i );
}

/**
 * Slot executed when the user wants to close the current tab
 */
void NMGMainTabWidget::deleteCurrentTab()
{
	emit closeCurrentTab();
}

/**
 * Slot executed when the user wants to close all the tabs
 */
void NMGMainTabWidget::deleteAllTabs()
{
	emit closeAllTabs();
}

/**
 * Slot executed when the user wants to close all the tabs but not hte current
 */
void NMGMainTabWidget::deleteOtherTabs()
{
	emit closeOtherTabs();
}

/**
 * Sets the image of the i tab
 * @param i Index of the tab
 * @param image Path of the image
 */
void NMGMainTabWidget::setImage( int i, const QString & image )
{
	setTabIcon( i, QIcon( *NMGImageManager::loadPixmap( image ) ) );
	tabBar()->repaint();
}

/**
 * Removes the image of the i tab
 * @param i Index of the tab
 */
void NMGMainTabWidget::removeImage( int i )
{
	setTabIcon( i, QIcon() );
	tabBar()->repaint();
}

/**
 * Returns the index of the tab of the widget
 * @param w Widget inside the scroll view
 * @return Index of the tab or -1 if there is no such tab
 */
int NMGMainTabWidget::getIndexFromWidgetInsideScrollView( QWidget *w )
{
	for ( int i = 0; i < count(); i++ )
		if ( getPageWidget( i ) == w ) return i;
	return -1;
}

#include "nmgmaintabwidget.moc"
