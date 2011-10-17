/***************************************************************************
 *   Copyright (C) 2005 by Pau Capella   *
 *   pcapella@ac.upc.edu   *
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
#ifndef NMINTERFACE_H
#define NMINTERFACE_H

#include <nminterpret.h>
#include <nmserversocket.h>
#include <nmmutex.h>
#include <list>

#define BUFFER 5120

/**
@author Pau Capella
@brief Implements the protocol to comunicate with the NetMeterGUI.
*/
class NMInterface : public NMServerSocket
{
	
	static NMInterface* interface;
public:

	static NMInterface *self();	
	
	~NMInterface();

	void newConnection ( NMSocket *socket );

        int sendCommand(NMString command);
	void closeClient();
	bool isAttach() const;
	
private:

	NMInterface(const char *name = 0, NMParent *nmparent = 0);

	int attachClient();
	void petitionClient();
	void send( NMString commnad);

	bool m_attach;
	NMSocket *m_client;	

	NMMutex m_mutex;
};

#endif
