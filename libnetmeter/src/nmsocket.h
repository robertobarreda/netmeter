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
#ifndef NMSOCKET_H
#define NMSOCKET_H

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <nmbuffer.h>
#include <nmevent.h>

class NMSocket;

class NMSocketListener: public NMParent {

friend class NMEventManager;

public:
	NMSocketListener(const char *name, NMSocket *parent);
	~NMSocketListener();
	
	NMSocket *socket();
private:
	void activate();
};

/**
\brief Provide acces for all kind of sockets.
@author Pau Capella
*/
class NMSocket : public NMBuffer
{	
public:
	NMSocket(const char *name = 0, bool listener = true, NMParent *parent = 0);
	NMSocket(const NMSocket &socket);
	virtual ~NMSocket();
	
	int open(int fd, NMFileMode mode);	
	void close();

	NMFileMode mode();
	int fd();
	bool isOpen();

	NMString readLine();
	NMString readWholeLine();
	void writeLine(const NMString &command, bool b_nl);
	
	int waitMS(int ms);
	
	int send(const void *buf, int len);
	int recv(void *buf, int len);

private:
	void readSocket();

public:
	NMEvent readyRead;

private:
	int m_fd; //Socket desctiptot
	NMFileMode m_mode; //ReadOnly, WriteOnly or ReadWrite
	NMSocketListener *m_socketlistener;
	bool m_listener;	
};



#endif
