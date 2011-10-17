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
#include "nmsocket.h"
#include "nmeventmanager.h"


/*************************** NMSocketListener ********************/

NMSocketListener::NMSocketListener(const char *name, NMSocket *parent)
  : NMParent(name, parent)
{
}

NMSocketListener::~NMSocketListener()
{
}

NMSocket *NMSocketListener::socket()
{
	return (NMSocket *)this->parent();
}

void NMSocketListener::NMSocketListener::activate()
{
	NMSocket *socket = (NMSocket *) this->parent();
	
	if(socket->waitMS(0) > 0) 
		socket->readyRead.activate();
}

/*************************** NMSocket ********************/

/**
\fn NMSocket::NMSocket(char *name, bool listener, NMParent *parent)
\brief Construct a new NMSocket 
\param listener true if you want to use events, otherwise false
*/
NMSocket::NMSocket(const char *name, bool listener, NMParent *parent)
: NMBuffer(name, parent),
  readyRead("readyRead",this),
  m_fd(-1),
  m_socketlistener(0),
  m_listener(listener)
{	
}

/**
\fn NMSocket::NMSocket(const NMSocket &socket)
\brief Construct a new NMSocket from socket
*/
NMSocket::NMSocket(const NMSocket &socket)
 : readyRead("readyRead",this)
{
	m_fd = socket.m_fd;
	m_mode = socket.m_mode;	
}

/**
\fn NMSocket::~NMSocket()
\brief Destroy current NMSocket
*/
NMSocket::~NMSocket()
{
	if( isOpen()) close();
}

/**
\fn NMSocket::open(int fd, NMBuffer::NMFileMode mode)
\brief Associte the socket to a exsisting file descriptor.
\param fd File descriptor.
\param mode Mode to open fd.
\return On success, fd is returned. On failure, a <0 is returned.
*/
int NMSocket::open(int fd, NMBuffer::NMFileMode mode)
{
	if(fd < 0) {
		cerr << "NMSocket open failed: This socket don't exsist." << '\n';
		return -1;
	}
	int rc, flags = 0;
	rc = fcntl(fd,F_GETFL,&flags);
	if(flags & O_NONBLOCK) {
		flags &= ~O_NONBLOCK;
		rc = fcntl(fd,F_SETFL,flags);
	}
	m_fd = fd;
	m_mode = mode;
	clear();

	if (m_listener) {
		m_socketlistener = new NMSocketListener("socklistener",this);
		NMEventManager::self()->registerSocketListener( m_socketlistener);
	}
	return m_fd;
}
/**
\fn NMSocket::close()
\brief Close the socket.
\return On success, zero is returned. On failure, a non-zero is returned.
*/
void NMSocket::close()
{
	if(m_fd >= 0) {
		if (m_listener) 
			NMEventManager::self()->unregisterSocketListener( m_socketlistener);
		::close(m_fd);
		m_fd = -1;
	}	//return rc;
}

/**
\fn NMSocket::readWholeLine()
\brief Read a line from socket.
\return Returns the text of the line read. Warning you can wait a long time for a line.
Use readLine() if for you is not important wait a whole line. Are you sure to use this methode?.
*/
NMString NMSocket::readWholeLine()
{
	if(m_fd < 0) {
		cerr << "NMSocket readWholeLine failed: Create or open a socket first." << '\n';
		return -1;
	}
	if(m_mode == WriteOnly) {
		cerr << "NMSocket readLine failed: You don't have permission for read this socket." << '\n';
		return -1;
	}
	
	char buf[32768];
	int pos;
	NMString str("");
	int contador = 0;

	memset(buf,'\0',32768);
	
	while (1) {
		if(length() > 0)
		{
			pos = search("\n",1);
			if (pos >= 0) {
				readB((void *)buf, pos+1);	
				str = buf;
				return str;
			}
		}
		readSocket();
		contador ++;
	}
	readB((void *)buf,length());
	str = buf;
	return str;
}

/**
\fn NMSocket::readLine()
\brief Read a line from socket.
\return Returns the text of the line read. Warning you can wait a severeal time for a line.
Use read() if you is not important wait a entire line.
*/
NMString NMSocket::readLine()
{
	if(m_fd < 0) {
		cerr << "NMSocket readLine failed: Create or open a socket first." << '\n';
		return -1;
	}
	if(m_mode == WriteOnly) {
		cerr << "NMSocket readLine failed: You don't have permission for read this socket." << '\n';
		return -1;
	}
	
	char buf[32768];
	int pos;
	NMString str("");
	int contador = 1;

	memset(buf,'\0',32768);
	
	//Fem tres voltes al bucle per intentar la nova linia.
	//Si no ho conseguim retornem lo que tenim i sortim....
	while (contador < 3) {
		if(length() > 0)
		{
			pos = search("\n",1);
			if (pos >= 0) {
				readB((void *)buf, pos+1);	
				str = buf;
				return str;
			}
		}
		readSocket();
		contador ++;
	}
	readB((void *)buf,length());
	str = buf;
	return str;
}
/**
\fn NMSocket::readSocket()
\brief Read directly of socket and put in the buffer.
*/
void NMSocket::readSocket()
{
	int nbytes;
	char buf[32768];
	memset(buf,'\0',32768);
	int pos=0;
	int rc;
	if(m_fd < 0) return;
	while (1)
	{
		struct timeval tv;
		tv.tv_sec = 1; 
		tv.tv_usec = 0;
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(m_fd,&fds);

		rc = select(m_fd+1, &fds, NULL, NULL, &tv);
		if(rc <= 0) break;
		
		nbytes = read(m_fd, buf, 32768);
		//cout << "------------------- BUFFER ------------------\n" << buf << "\n--------------------------" << '\n';
		if (nbytes == -1)
		{
			if (errno == EAGAIN || errno == EINTR) {
				continue;
			}
			else {
				m_fd = -1;
				break;
			}
		}
		else if (nbytes == 0) return;

		//assert(nbytes != 0);
		writeB(buf,nbytes);

		memset(buf,'\0',32768);
		pos = search("\n",1);
		if(pos >= 0) break;
	}
}


/**
\fn NMSocket::writeLine(const NMString &command, bool b_nl)
\brief Write line to socket.
\param command Text to write.
\param b_nl Add newline to end the text.
*/
void NMSocket::writeLine(const NMString &command, bool b_nl)
{
	int rc;
	
	if(m_fd < 0) {
		cerr << "NMSocket writeLine failed: Create or open a file first." << '\n';
		return;
	}
	if(m_mode == ReadOnly) {
		cerr << "NMSocket writeLine failed: You don't have permission for write this socket." << '\n';
		return;
	}
	
	NMString nms_request;
	if(b_nl == true)
		nms_request = command + "\n";
	rc=write(m_fd, nms_request.c_str(), nms_request.Length());
	
}
/**
\fn NMSocket::waitMS(int ms)
\brief Wait for socket can read. 
\param ms Is the time elapsed before waitMS returns.
\return On success, fd is returned. On failure, a -1 is returned.
*/
int NMSocket::waitMS(int ms)
{
	int rc;
	int flags = 0;

	rc = fcntl(m_fd,F_GETFL,&flags);
	if(flags & O_NONBLOCK) {
		flags &= ~O_NONBLOCK;
		rc = fcntl(m_fd,F_SETFL,flags);
	}
	//Tenim algo en el buffer
	if(length() > 0) return m_fd;

	if(m_fd < 0) return -1;
	
	struct timeval tv;
	tv.tv_sec = 0; 
	tv.tv_usec = 1000*ms;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_fd,&fds);
	rc = select(m_fd+1, &fds, NULL, NULL, &tv);
	if(rc > 0) {
		readSocket();
		if(length() > 0) return m_fd;
	}
	return -1;
	
}	
/**
\fn NMSocket::fd()
\brief Return the file descriptor of the current socket. 
*/
int NMSocket::fd()
{
	return m_fd;
}

/**
\fn NMSocket::mode()
\brief Return the mode of the current socket.
*/
NMBuffer::NMFileMode NMSocket::mode()
{
	return m_mode;
}

/**
\fn NMSocket::send(const void *buf, int len)
\brief Transmit a message to the socket.
\param buf Data to send.
\param len Data size.	
\return On success, return the number of characters sent. On error, -1 is returned,
*/
int NMSocket::send(const void *buf, int len)
{
	int rc;
	if(m_fd < 0) {
		cerr << "NMSocket send failed: Create or open a file first." << '\n';
		return -1;
	}
	if(m_mode == ReadOnly) {
		cerr << "NMSocket send failed: You don't have permission for write this socket." << '\n';
		return -1;
	}
	rc = ::send (m_fd, buf, len, 0);
	if(rc < 0) {
		cerr << "NMSocket send failed: " << strerror(errno) << '\n';
		return rc;
	}
	return rc;
}

/**
\fn NMSocket::recv(void *buf, int len)
\brief Receive a message from a socket.
\param buf Data
\param len Data size.	
\return On success, return the number of bytes received, or -1 if an error occurred.
*/
int NMSocket::recv(void *buf, int len)
{
	int rc;	
	if(m_fd < 0) {
		cerr << "NMSocket recv failed: Create or open a file first." << '\n';
		return -1;
	}
	if(m_mode == WriteOnly) {
		cerr << "NMSocket recv failed: You don't have permission for write this socket." << '\n';
		return -1;
	}
	
	rc = readB((void *)buf, len);
	if(rc <= 0) {
		readSocket();
		rc = readB((void *)buf, len);
		if(rc <= 0) {
			cerr << "NMSocket recv failed: " << strerror(errno) << '\n';
			return rc;
		}
	}	
	return rc;
}

/**
\fn NMSocket::isOpen()
\brief Is the socket open.
\return Return TRUE if socket is open, otherwise return FALSE.
*/
bool NMSocket::isOpen()
{
	if (m_fd < 0) return false;
	return true;
}
