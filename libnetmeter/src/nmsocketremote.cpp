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
#include "nmsocketremote.h"

/**
\fn NMSocketRemote(char *name, NMSocketRemoteType type, NMParent *nmparent)
\brief Creates a NMSocketRemite object for a stream or datagram socket.
\param type Must be either NMSocketRemote::stream for TCP sockets or NMSocketRemote::datagram for UDP sockets
*/
NMSocketRemote::NMSocketRemote(const char *name, NMSocketRemoteType type, NMParent *nmparent)
 : NMSocket(name, false, nmparent),
m_type(type), m_port(0), m_host("0.0.0.0")
{
	int sock;
	memset (&myaddr, 0, sizeof(myaddr));

	if(this->fd() >= 0) {
		cerr << "NMSocketRemote create failed: Socket is used." << '\n';
		return;
	}

	if(type == NMSocketRemote::stream)
		sock = ::socket(AF_INET, SOCK_STREAM, 0);
	else //NMSocketRemote::datagram
		sock = ::socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0)
        {
                cerr << "NMSocketRemote create failed: " << strerror(errno) << '\n';
                return;
        }
	open(sock, NMBuffer::ReadWrite);
}

/**
\fn NMSocketRemote(char *name, int socket, NMSocketRemoteType type, NMParent *nmparent)
\brief Creates a NMSocketRemite object for the exsisting socket socket
\param socket An exsisting socket
\param type Must be either NMSocketRemote::stream for TCP sockets or NMSocketRemote::datagram for UDP sockets
*/
NMSocketRemote::NMSocketRemote(const char *name, int socket, NMSocketRemoteType type, NMParent *nmparent)
: NMSocket(name, true, nmparent),
m_type(type), m_port(0), m_host("0.0.0.0")
{
	memset (&myaddr, 0, sizeof(myaddr));
	open(socket, NMBuffer::ReadWrite);
}


NMSocketRemote::~NMSocketRemote()
{
	memset (&myaddr, 0, sizeof(myaddr));
}


/**
\fn NMSocketRemote::bind( unsigned int port)
\brief Is used by servers for setting up incoming connections.
\param port port number
\return On success, zero is returned. On error, -1 is returned.
*/
int NMSocketRemote::bind( unsigned int port)
{
	int rc;

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(port);
	myaddr.sin_addr.s_addr =  INADDR_ANY; 
	bzero(&(myaddr.sin_zero),8);

	rc =::bind(fd(),(struct sockaddr *)&myaddr,sizeof(struct sockaddr));
	if(rc < 0) {
		cerr << "NMSocketRemote bind failed: " << strerror(errno) << '\n';
		return -1;
	}

	m_port = port;
	m_host = NMHost("127.0.0.1");

	return rc;
}


/**
\fn NMSocketRemote::listen()
\brief Listen for connections on a socket.
\param backlog Maximum allowed connections.
\return On success, zero is returned. On error, -1 is returned.
*/
int NMSocketRemote::listen(int backlog)
{
	int rc;
	rc =::listen(this->fd(), backlog);
	if(rc < 0) {
		cerr << "NMSocketRemote listen failed: " << strerror(errno) << '\n';
		return -1;
	}
	return rc;
}

/**
\fn NMSocketRemote::accept()
\brief Accept a connection on a socket
\return On success, Client socket is returned, in mode WriteOnly.
*/
NMSocket *NMSocketRemote::accept()
{
	int fdclient;
	struct sockaddr_in addr_client;
	int addr_len = sizeof(struct sockaddr_in);
	
	fdclient =::accept(fd(), (struct sockaddr *)&addr_client,(socklen_t *) &addr_len);
	if(fdclient < 0) {
		cerr << "NMSocketRemote accept failed: " << strerror(errno) << '\n';
		return 0;
	}

	cout << "IP del client..." << inet_ntoa(addr_client.sin_addr) << '\n'; 

	NMSocket *client = new NMSocket("client", true, this);
	client->open(fdclient, NMBuffer::ReadWrite);
	return client;
}

/**
\fn NMSocketRemote::connect(NMHost host, unsigned int port)
\brief Connects to the IP address and port specified by host and port.
\return On success, zero is returned.  On error, -1 is returned.
*/
int NMSocketRemote::connect(NMHost host, unsigned int port)
{

	//Omplim la estrucutra del socket...
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(port);
	myaddr.sin_addr.s_addr = inet_addr(host.getAddress().c_str());
	bzero(&(myaddr.sin_zero),8); 

	int rc;
	rc =::connect(fd(), (struct sockaddr *)&myaddr, sizeof(struct sockaddr));
	if(rc < 0) 
		cerr << "NMSocketRemote connect failed: " << strerror(errno) << '\n';
	
	m_port = port;
	m_host = host;

	return rc;
}

/**
\fn NMSocketRemote::port()
\brief Returns the port of this socket device
*/
unsigned int NMSocketRemote::port()
{
	return m_port;
}

/**
\fn NMSocketRemote::host()
\brief Returns the address of this socket device
*/
NMHost NMSocketRemote::host()
{
	return m_host;
}
