/*
 *   $Id: nmserversocket.cpp,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
 *
 *   Copyright (c) 2006 Pau Capella
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "nmserversocket.h"

/**
\fn NMServerSocket::NMServerSocket(char *name, unsigned int port, unsigned int backlog, NMParent *parent )
\brief Creates a server socket object, that will serve the given port on all the addresses of this host. Use backlog to specify how many pending connections the server can have.
*/
NMServerSocket::NMServerSocket( unsigned int port, unsigned int backlog, const char *name, NMParent *parent )
 : NMParent( name, parent), NMThread(),
activated ( "nme_activated", this),
m_port( port), m_backlog( backlog), m_ok( false)
{
	init();
}

/**
\fn NMServerSocket::~NMServerSocket()
\brief Destroy the socket
*/
NMServerSocket::~NMServerSocket()
{
	delete m_socket;
}


void NMServerSocket::init()
{
	m_socket = new NMSocketRemote( "nmsr_server", NMSocketRemote::stream, this);

	if (m_socket->bind( m_port) < 0 || m_socket->listen( m_backlog) < 0) return;

	activated.connect( this, &NMServerSocket::incomingConnection);

	m_ok = true;

	start();
}

void NMServerSocket::run()
{
	NMSocket *sock = 0;
	sock = m_socket->accept();
	if( !sock) return;

	activated.setValue( sock);
	activated.activate();
}

void NMServerSocket::incomingConnection()
{
	NMSocket *socket;
	socket = static_cast<NMSocket *>( activated.value());
	//Excecutem el new connection
	start();
	this->newConnection( socket);
}

/**
\fn NMServerSocket::ok() const
\brief Returns TRUE if the construction succeeded; otherwise returns FALSE.
*/
bool NMServerSocket::ok() const
{
	return m_ok;
}

/**
\fn NMServerSocket::port() const
\brief Returns the port of this socket device
*/
unsigned int NMServerSocket::port() const
{
	return m_port;
}
