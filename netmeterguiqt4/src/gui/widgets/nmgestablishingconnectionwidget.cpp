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
#include "nmgestablishingconnectionwidget.h"

#include <QLabel>
#include <QLayout>
#include <QTimer>
#include <QApplication>
#include <QVBoxLayout>
#include <QDesktopWidget>

#include <nmgapplicationinformation.h>

/**
 * Contructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGEstablishingConnectionWidget::NMGEstablishingConnectionWidget ( QWidget *parent, const char *name )
		: QWidget ( parent )
{
	if ( !name )
		setObjectName ( "NMGEstablishingConnectionWidget" );
	else
		setObjectName ( name );
	setWindowTitle ( "Establishing connection..." );
	QVBoxLayout * NMGEstablishingConnectionWidgetLayout = new QVBoxLayout ( this );
	NMGEstablishingConnectionWidgetLayout->setContentsMargins ( 11,11,11,11 ); //This line is optional since default margin is 11
	NMGEstablishingConnectionWidgetLayout->setSpacing ( 6 );
	NMGEstablishingConnectionWidgetLayout->setObjectName ( "NMGEstablishingConnectionWidgetLayout" );

	QLabel * textLabel1 = new QLabel ( this );
	textLabel1->setObjectName ( "textLabel1" );
	textLabel1->setText ( "Establishing connection with the server..." );

	NMGEstablishingConnectionWidgetLayout->addWidget ( textLabel1 );

	resize ( QSize ( 266, 53 ).expandedTo ( minimumSizeHint() ) );
	ensurePolished ();


}

/**
 * Destructor of the class
 * @return
 */
NMGEstablishingConnectionWidget::~NMGEstablishingConnectionWidget()
{}

/**
 * Shows the widget (and disappears a second after)
 */
void NMGEstablishingConnectionWidget::show()
{
	QWidget * mainW = ( QWidget * ) NMGApplicationInformation::getApplicationInformation()->getMainWindow();
	int x, y;
	if ( mainW )
	{
		int wd = mainW->geometry().width();
		int wh = mainW->geometry().height();
		int wx = mainW->geometry().x();
		int wy = mainW->geometry().y();
		x = wx + ( wd - 266 ) / 2;
		y = wy + ( wh - 53 ) / 2;
	}
	else
	{
		QDesktopWidget* d = QApplication::desktop();
		int w = d->width();     // returns desktop width
		int h = d->height();    // returns desktop height
		x = ( w - 266 ) / 2;
		y = ( h - 53 ) / 2;
	}

	if ( !mainW ) return;

	setGeometry ( x, y, 266, 53 );

	QWidget::show();
	QTimer::singleShot ( 1000, this, SLOT ( hideWidget() ) );
}

/**
 * Hides the widget
 */
void NMGEstablishingConnectionWidget::hide()
{
	QWidget::hide();
}


/**
 * Slot which hides the widget
 */
void NMGEstablishingConnectionWidget::hideWidget()
{
	hide();
}


#include "nmgestablishingconnectionwidget.moc"
