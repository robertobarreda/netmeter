/*
 *   $Id: nmftp.h,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
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
#ifndef NMFTP_H
#define NMFTP_H

#include <list>
#include <nmparent.h>
#include "nmexternallocal.h"
#include "nmevent.h"

class NMFtpCommand;

/**
	@author Pau Capella <pau.capella@ac.upc.edu>
	@brief  Provides an implementation of the SFTP protocol.
*/
/**	
	<b> Exemple to usage </b>\n
	\code
	NMFtp *sftp = new NMFtp("sftp",0);
	sftp->connectToHost( "host", "username", "password");
	sftp->cd("/tmp");
	sftp->get("results.txt");
	sftp->remove("results.txt");
	sftp->close();
	\endcode
*/

class NMFtp : public NMParent
{
public:
	/**
	\enum State
	\param Unconnected There is no connection to the host.
	\param Connected Connection to the host has been achieved.
   	*/
	enum State {
		Unconnected,
		PasswordSend,
		Connected,
	};

	/**
	\enum Command
	\param None No command is begin executed
	\param Cd cd() is being executed.
	\param Get get() is begin executed
	\param Rmdir rmdir() is begin executed
	\param Mkdir mkdir() is begin executed
	\param Remove rm() is begin executed
	\param Exit close() is begin executed
   	*/
	enum Command {
		None,
		Cd,
		Put,
		Get,
		Rmdir,
		Mkdir,
		Remove,
		Exit
	};

	/**
	\enum Error
	\param NoError No error occurred.
	\param TimeOut Timeout connection
	\param WrongPassword Wrong password
	\param IsConnected The host is connected
	\param Unknow An error other than those specified above occurred
   	*/
	enum Error {
		NoError = 0,
		Timeout,
		WrongPassword,
		IsConnected,
		Unknow
	};

	NMFtp( const char *name, NMParent *parent);
	~NMFtp();

	Error connectToHost( const NMString &host, const NMString &username, const NMString &password, int hostPort=22, bool noHostKeyCheck=false);
	void close();
	
	void put( const NMString &file);
	void get( const NMString &file);
	void cd( const NMString &path);
	void remove( const NMString &file);
	void mkdir( const NMString &dir);
	void rmdir( const NMString &dir);

	Command currentCommand();
	Command lastCommand();
	
	/**
	This event is emitted when processing the command identified currentCommand()
	*/
	NMEvent commandStarted;
	
	/**
	This event is emitted when the command identified lastCommand() has finished
	*/
	NMEvent commandFinished;
	
private:
	void startNextCommand();
	Error converseSFtp();

	//callback
	void processRecived();

	NMExternalLocal *m_sftp;

	State m_state;
	Command m_currentcommand;
	Command m_lastcommand;
	NMString m_host;
	int hostPort;
	NMString m_password;
	NMString m_username;

	list<NMFtpCommand *>  *m_pending; //Commandes a pendents d'execuci�};
};

class NMFtpCommand
{
public:
	friend class NMFtp;

	NMFtpCommand( NMFtp::Command command, const NMString args) :m_command(command), m_args(args) {}
	~NMFtpCommand();
private:
 	NMFtp::Command m_command;
	NMString m_args;
};

#endif
