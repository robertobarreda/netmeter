 /*
 *   $Id: nmftp.cpp,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
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
#include "nmftp.h"
#include <stdlib.h>

/**
\fn NMFtp::NMFtp( const char *name, NMParent *parent)
\brief Creates a new NMFtp object
*/
NMFtp::NMFtp( const char *name, NMParent *parent)
 : NMParent(name, parent),
   commandStarted("ftp_command_started", this),
   commandFinished("ftp_command_finished",this),
   m_sftp(0), m_state( NMFtp::Unconnected ),
   m_currentcommand( NMFtp::None ), m_lastcommand( NMFtp::None )
{
	m_pending = new list<NMFtpCommand *>;
}

/**
\fn NMFtp::~NMFtp()
\brief Destroy NMFtp object
*/
NMFtp::~NMFtp()
{
	m_pending->clear();
	delete m_pending;

// 	if(m_sftp) delete m_sftp;
// 	m_sftp = 0;
}

/**
\fn NMFtp::connectToHost( const NMString &host, const NMString &username, const NMString &password)
\brief Connects to the SFtp server host using username username and password password
*/
NMFtp::Error NMFtp::connectToHost( const NMString &host, const NMString &username, const NMString &password, int hostPort, bool noHostKeyCheck)
{
	if(m_state != NMFtp::Unconnected)
		return NMFtp::IsConnected;

	m_host = host;
	if( username.IsEmpty())
		m_username = getenv("USER");
	else
		m_username = username;
	m_password = password;
	
	
	//sftp -oPort=1234 root@localhost    
	m_sftp = new NMExternalLocal("sftp", true, this);
	*m_sftp << "sftp";
	if( hostPort != 22) {
		NMString portParameter = "-oPort=" + NMString::number(hostPort);
		*m_sftp <<  portParameter;
	}
	if (noHostKeyCheck) {
		*m_sftp <<  "-oStrictHostKeyChecking=no";
	}
	*m_sftp << username + "@" + host;

	#ifdef DEBUG
		cout << "sftp command: " << m_sftp->shellCommand() << endl;
	#endif
	//TODO Tractament d'errors!
	if( m_sftp->run() != NMExternal::NoError)
		return NMFtp::Unknow;
	
	Error rc = converseSFtp();
	if( rc != NMFtp::NoError)
		return rc;
	
	m_sftp->readyReadOutput.connect( this, &NMFtp::processRecived);
	return NMFtp::NoError;
}

/**
\fn NMFtp::converseSFtp()
\brief Try to login to SFtp server
*/
NMFtp::Error NMFtp::converseSFtp()
{
	while ( m_state != NMFtp::Connected) {
	
		if( m_sftp->stdInOut.waitMS(24000) < 0)
			return NMFtp::Timeout;

		NMString line = m_sftp->stdInOut.readLine();

		if( line.search( "Password:") >= 0 || line.search( "password:") >= 0) {
			if( m_state == PasswordSend) {
				m_sftp->kill();
				return NMFtp::WrongPassword;
			}
			m_sftp->stdInOut.writeLine( m_password, true);
			m_state = PasswordSend;
		}
		else if( line.search( "sftp>") >= 0) {
			m_state = Connected;
			return NMFtp::NoError;
		}
                // (rbarreda) For debain ssh bug compatibility. More info: DSA-1571-1 openssl -- predictable random number generator
                else if ( line.search ( "blacklisted" ) >= 0 ) continue;
	}
	return NMFtp::Unknow;
}

void NMFtp::processRecived()
{
	NMString line = m_sftp->stdInOut.readLine();
	
	if( line.search("sftp>") >= 0 && m_currentcommand != NMFtp::None) {
		m_lastcommand = m_currentcommand;
		m_currentcommand = NMFtp::None;
		NMFtpCommand *cmd = m_pending->front();
		if(cmd)
		{
			commandFinished.activate();
			m_pending->pop_front();
		}
		startNextCommand();
	}
}


/**
\fn NMFtp::close()
\brief Closes the connection to the SFtp server.
*/
void NMFtp::close()
{
	m_pending->push_back( new NMFtpCommand( NMFtp::Exit, ""));
	if( m_pending->size() == 1)
		startNextCommand();
}

/**
\fn NMFtp::put(const NMString &file)
\brief Upload the file file to the server.
*/
void NMFtp::put(const NMString &file)
{
	m_pending->push_back( new NMFtpCommand( NMFtp::Put, file));
	if( m_pending->size() == 1)
		startNextCommand();
}

/**
\fn NMFtp::get(const NMString &file)
\brief Download the file file from the server.
*/
void NMFtp::get(const NMString &file)
{
	m_pending->push_back( new NMFtpCommand( NMFtp::Get, file));
	if( m_pending->size() == 1)
		startNextCommand();
}

/**
\fn NMFtp::cd( const NMString &path)
\brief Changes the working directory of the server to dir.
*/
void NMFtp::cd( const NMString &path)
{
	m_pending->push_back( new NMFtpCommand( NMFtp::Cd, path));
	if( m_pending->size() == 1)
		startNextCommand();
}

/**
\fn NMFtp::remove( const NMString &file)
\brief Deletes the file file from the server
*/
void NMFtp::remove( const NMString &file)
{
	m_pending->push_back( new NMFtpCommand( NMFtp::Remove, file));
	if( m_pending->size() == 1)
		startNextCommand();
}

/**
\fn NMFtp::mkdir( const NMString &dir)
\brief Creates a directory called dir on the server.
*/
void NMFtp::mkdir( const NMString &dir)
{
	m_pending->push_back( new NMFtpCommand( NMFtp::Mkdir, dir));
	if( m_pending->size() == 1)
		startNextCommand();
}

/**
\fn NMFtp::rmdir( const NMString &dir)
\brief Deletes the directory dir from server
*/
void NMFtp::rmdir( const NMString &dir)
{
	m_pending->push_back( new NMFtpCommand( NMFtp::Rmdir, dir));
	if( m_pending->size() == 1)
		startNextCommand();
}


/**
\fn NMFtp::currentCommand()
\brief Returns the command type of the FTP command being executed or None if there is no command being executed.
*/
NMFtp::Command NMFtp::currentCommand()
{
	return m_currentcommand;
}

/**
\fn NMFtp::currentCommand()
\brief Returns the last FTP command executed or None if there is no command being executed.
*/
NMFtp::Command NMFtp::lastCommand()
{
	return m_lastcommand;
}


void NMFtp::startNextCommand()
{
	NMFtpCommand *cmd = m_pending->front();
	if( !cmd) return;

	m_currentcommand = cmd->m_command;
	commandStarted.activate();
	
	switch ( m_currentcommand) {
		case NMFtp::Cd:
			m_sftp->stdInOut.writeLine( "cd " + cmd->m_args, true);
			break;
		case NMFtp::Put:
			m_sftp->stdInOut.writeLine( "put " + cmd->m_args, true);
			break;
		case NMFtp::Get:
			m_sftp->stdInOut.writeLine( "get " + cmd->m_args, true);
			break;
		case NMFtp::Remove:
			m_sftp->stdInOut.writeLine( "rm " + cmd->m_args, true);
			break;
		case NMFtp::Mkdir:
			m_sftp->stdInOut.writeLine( "mkdir " + cmd->m_args, true);
			break;
		case NMFtp::Rmdir:
			m_sftp->stdInOut.writeLine( "rmdir " + cmd->m_args, true);
			break;
		case NMFtp::Exit:
			m_sftp->stdInOut.writeLine( "exit", true);
			if( m_state == Connected) 
				m_state = Unconnected;
			break;
 		default:
			break;
	};
}
