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
#include "nmgexemplesubscriptedwidget.h"

#include <qpushbutton.h>

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGExempleSubscriptedWidget::NMGExempleSubscriptedWidget ( NMGISender * s, NMGIModuleCommunication * mc, QWidget *parent, const char *name )
		: QWidget ( parent )
{
	setObjectName ( name );
	setupUi ( this );

	sender = s;
	moduleCommunication = mc;
	connect ( buttonStart, SIGNAL ( clicked() ), this, SLOT ( buttonStartClicked() ) );
	connect ( buttonStatus, SIGNAL ( clicked() ), this, SLOT ( buttonStatusClicked() ) );
	connect ( buttonStop, SIGNAL ( clicked() ), this, SLOT ( buttonStopClicked() ) );
	connect ( buttonSubscribe, SIGNAL ( clicked() ), this, SLOT ( buttonSubscribeClicked() ) );
}

/**
 * Destructor of the class
 * @return
 */
NMGExempleSubscriptedWidget::~NMGExempleSubscriptedWidget()
{}

void NMGExempleSubscriptedWidget::buttonStartClicked()
{
	cout << "Start" << endl;
	sender->sendCommand ( Start, "" );
}

void NMGExempleSubscriptedWidget::buttonStopClicked()
{
	cout << "Stop" << endl;
	sender->sendCommand ( Stop, "" );
}

void NMGExempleSubscriptedWidget::buttonStatusClicked()
{
	cout << "Status" << endl;
	sender->sendCommand ( Status, "" );
}

void NMGExempleSubscriptedWidget::buttonSubscribeClicked()
{
	cout << "Subscribe" << endl;
	QString id = moduleCommunication->subscribe ( "NMGExempleSubscriptor", "<data>nothing</data>" );
	if ( id.isEmpty() ) cerr << "An error occurred while subscribing NMGExempleSubscriptor to NMGExempleSubscripted" << endl;
	else cout << "NMGExempleSubscriptor subscibed to NMGExempleSubscripted" << endl;
}

#include "nmgexemplesubscriptedwidget.moc"
