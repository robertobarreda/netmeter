/***************************************************************************
 *   Copyright (C) 2005 by Joan Sala                                       *
 *   jsala@ac.upc.edu                                                      *
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

#include "nmgsyncremote.h"

#include <iostream>
using namespace std;

// Default constructor & destructor 

NMGSyncRemote::NMGSyncRemote ( NMGISender * s, QString currentTestId )
		: sender ( s )
{
	setObjectName ( "NMGSyncRemote" );
	testId = currentTestId;
}

NMGSyncRemote::~NMGSyncRemote ( )
{
}

// NMSync Asynchronous plugin comands:

void NMGSyncRemote::sendStart( QString data )
{
	sender->sendCommand(Start, data);
}

void NMGSyncRemote::sendStop( )
{
	sender->sendCommand(Stop, "");
}

#include "nmgsyncremote.moc"

