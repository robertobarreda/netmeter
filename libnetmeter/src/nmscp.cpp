/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
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
#include "nmscp.h"
#include <stdlib.h>

NMScp::~NMScp()
{
	if ( m_scp )
	{
		m_scp->stdInOut.close();
		delete m_scp;
		m_scp = 0;
	}
}

NMExternal::Error NMScp::put ( const NMString & localFile, const NMString & remoteFile )
{
	// Put user environment's identifier is no user is set.
	if ( m_username.IsEmpty() )
		m_username = getenv ( "USER" );

	if ( m_host.isNull() ) return NMExternal::NoHost;

	NMString m_command = localFile + " " + m_username + "@" + m_host.getAddress() + ":" + remoteFile;
	return runScp ( m_command );
}

NMExternal::Error NMScp::get ( const NMString & remoteFile, const NMString & localFile )
{
	// Put user environment's identifier is no user is set.
	if ( m_username.IsEmpty() )
		m_username = getenv ( "USER" );

	if ( m_host.isNull() ) return NMExternal::NoHost;

	NMString m_command = m_username + "@" + m_host.getAddress() + ":" + remoteFile + " " + localFile;
	return runScp ( m_command );
}

NMExternal::Error NMScp::runScp ( const NMString & m_command )
{
	NMExternal::Error rc;

	// Ignore all signals recieved:
	for ( int sig = 10; sig < NSIG; sig++ ) signal ( sig,SIG_IGN );

	/*if( m_pid > 0) 
	{
		cerr << "NMExternalRemote run failed: The process is already runnnig" << '\n';
		return NMExternal::IsRunning; 
	}*/

	if ( m_host.isNull() )
	{
		cerr << "NMScp run failed: Host is not defined" << endl;
		return NMExternal::NoHost;
	}

	if ( !m_scp ) m_scp = new NMExternalLocal ( "scp", false, this );

	//*m_scp << "exec scp";
	*m_scp << "scp";
	//if ( m_setQuietMode ) *m_scp << "-q"; // (rbarreda)=> It's necessary to mantain a minimum verbose level to know
                                                //              if the file is transfered properly (feedback of "100%" status transfer)
	if ( m_setCompression ) *m_scp << "-C";
	if ( m_recursivelyCopyDirectory ) *m_scp << "-r";
	if ( m_port != 22 ) *m_scp << "-P " + m_port;
	if ( m_noHostKeyCheck )	*m_scp <<  "-oStrictHostKeyChecking=no";

	*m_scp << m_command; // PUT or GET command

	#ifdef DEBUG
		cout << "NMScp::run with command " << m_scp->shellCommand() << endl;
	#endif

	if ( ( rc = (NMExternal::Error) m_scp->run() ) != NMExternal::NoError )
	{
		m_scp->stdInOut.close();
		m_scp->killsignal();
		delete m_scp;
		m_scp = 0;
		return rc;
	}

	if ( ( rc = converseScp() ) != NMExternal::NoError )
	{
		m_scp->stdInOut.close();
		m_scp->killsignal();
		delete m_scp;
		m_scp = 0;
		return rc;
	}

	//m_isrun = true;
	//m_pid = m_scp->pid();
	//stdInOut.open(m_scp->stdInOut.fd(), NMBuffer::ReadWrite);

	if ( m_listener )
	{
		m_externallistener = new NMExternalListener ( "locallistener", this );
		NMEventManager::self()->registerExternalListener ( m_externallistener );
	}

	return NMExternal::NoError;
}

NMExternal::Error NMScp::converseScp()
{
	NMString line;
	bool isLoggedIn = false;

	while ( !isLoggedIn )
	{
		if ( m_scp->stdInOut.waitMS ( 30000 ) < 0 )
		{
			cerr << "NMScp establishConnection failed: Timeout in ssh connexion" << '\n';
			return NMExternal::Timeout;
		}

		line = m_scp->stdInOut.readLine();

	        if ( line.search ( "(yes/no)?" ) >= 0 ) m_scp->stdInOut.writeLine ( "yes", true );
		else if ( line.search ( "Password:" ) >= 0 || line.search ( "password:" ) >= 0 )
		{
			m_scp->stdInOut.writeLine ( m_password, true );

			line = m_scp->stdInOut.readLine();
                        if ( line.search ( "Password:" ) >= 0 || line.search ( "password:" ) >= 0 ) return NMExternal::WrongPassword;
			else isLoggedIn = true;
		}
		else if ( line.search ( "lost connection" ) >= 0 || line.search ( "usage" ) >= 0 ) return NMExternal::Unknow;
                else if ( line.search ( "100%" ) >= 0 ) isLoggedIn = true;
                // (rbarreda) For debain ssh bug compatibility. More info: DSA-1571-1 openssl -- predictable random number generator
                else if ( line.search ( "blacklisted" ) >= 0 ) continue;
		else return NMExternal::Unknow;
	}
	return NMExternal::NoError;
}
