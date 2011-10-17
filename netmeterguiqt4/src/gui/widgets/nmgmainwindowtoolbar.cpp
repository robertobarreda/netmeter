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
#include "nmgmainwindowtoolbar.h"
#include "nmgmainwindow.h"

#include <nmgimagemanager.h>
#include <nmgpreferencesmanager.h>

/**
 * Contructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGMainWindowToolBar::NMGMainWindowToolBar( NMGMainWindow *parent, const char *name )
		: QToolBar( "NMGMainWindowToolBar", parent )
{
	if ( name )
		setObjectName( name );
	else
		setObjectName( "NMGMainWindowToolBar" );

	toolButtonConnect = addAction( *NMGImageManager::loadPixmap( "connect.png" ), "Connect to server", parent, SLOT( connectToServer() ) );
	toolButtonConnect->setToolTip( "Connect to server" );
	toolButtonConnect->setStatusTip( "Connect to server" );

	toolButtonDisconnect = addAction( *NMGImageManager::loadPixmap( "disconnect.png" ), "Disconnect from server", parent, SLOT( disconnectFromServer() ) );
	toolButtonDisconnect->setToolTip( "Disconnect from server" );
	toolButtonDisconnect->setStatusTip( "Disconnect from server" );

	configureButton = addAction( *NMGImageManager::loadPixmap( "configure.png" ), "Configure NetMeter", NMGPreferencesManager::self(), SLOT( showGeneralPreferencesWidget() ) );
	configureButton->setToolTip( "Configure NetMeter" );
	configureButton->setStatusTip( "Configure application" );

	exitButton = addAction( *NMGImageManager::loadPixmap( "exit.png" ), "Quit", parent, SLOT( quitApplication() ) );
	exitButton->setToolTip( "Quit" );
	exitButton->setStatusTip( "End application" );

	setConnected( false );
}

/**
 * Destroyer of the class
 * @return
 */
NMGMainWindowToolBar::~NMGMainWindowToolBar()
{
}

/**
 * Changes the buttons according to the status of the connection (true==connected, false==!connected)
 * @param connected
 */
void NMGMainWindowToolBar::setConnected( bool connected )
{
	if ( connected )
	{
		toolButtonConnect->setEnabled( false );
		toolButtonDisconnect->setEnabled( true );
	}
	else
	{
		toolButtonConnect->setEnabled( true );
		toolButtonDisconnect->setEnabled( false );

	}
}

#include "nmgmainwindowtoolbar.moc"
