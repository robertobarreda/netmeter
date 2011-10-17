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
#ifndef NMSOCKETREMOTE_H
#define NMSOCKETREMOTE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>  
#include <netinet/in.h>
#include <arpa/inet.h>

#include "nmsocket.h"
#include "nmhost.h"


/**
@author Pau Capella
@brief Provide acces to low level socket api
*/
class NMSocketRemote : public NMSocket
{
public:
	/**
	\enum NMSocketRemoteType
	\param datagram Provides a connection UDP socket
	\param stream Provides a connection-oriented TCP socket
	*/

	enum NMSocketRemoteType {
		datagram,
		stream
	};
	

	NMSocketRemote(const char *name, NMSocketRemoteType type, NMParent *nmparent);
	NMSocketRemote(const char *name, int socket, NMSocketRemoteType type, NMParent *nmparent);

	~NMSocketRemote();

	int bind( unsigned int port);
	int listen(int backlog);
	NMSocket *accept();
	int connect(NMHost host, unsigned int port);

	unsigned int port();
	NMHost host();

private:

	struct sockaddr_in myaddr;
	
	NMSocketRemoteType m_type;
	unsigned int m_port;
	NMHost m_host;
	

};

#endif
