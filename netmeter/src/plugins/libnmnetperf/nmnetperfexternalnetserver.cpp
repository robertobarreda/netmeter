/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda                                 *
 *   rbarreda@ac.upc.edu                                                   *
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
#include "nmnetperfexternalnetserver.h"
#include "nmnetperfutils.h"

#include <nmstring.h>
#include <nmexternallocal.h>
#include <nmexternalremote.h>
#include <nmdns.h>

NMNetperfExternalNetserver::NMNetperfExternalNetserver ( NMParent * parent, NMNetperfErrorManager * errorMan,
        NMNetperfStatusManager * statusMan, NMNetperfEventNetserverRunning * netserverRun,
        NMNetperfEventFinishedNetserver * finNetserver )
		: NMParent ( "NMNetperfExternalNetserver", parent )
{
	errorManager = errorMan;
	statusManager = statusMan;
	netserverRunning = netserverRun;
	finishedNetserver = finNetserver;
	external = 0;
	isRunning = false;
}

bool NMNetperfExternalNetserver::configureAndRunNetserver ( NMNetperfInputFields * inputFields )
{
#ifdef DEBUG
	cout << LIGHTPURPLE << "(dest)> NMNetperfExternalNetserver::configureNetserver" << ENDCOLOR << endl;
#endif

	if ( external ) return false;

	statusManager->setStatus ( NMNetperfStatusManager::SendingNetserverScriptFile );

	NMHost destHost = NMDns::resolv ( inputFields->destManagementAddress,
	                                  ( inputFields->destManagementProtocol == "IPv6" ) ? NMDns::IPv6 : NMDns::IPv4 );

	if ( destHost.isNull() )
	{
#ifdef DEBUG
		cerr << RED << "[ERROR] Cannot resolve the destination address (" << inputFields->destManagementAddress << ")." << ENDCOLOR << endl;
#endif
		errorManager->notifyError ( NMError ( NMError::fatal, CANNOT_RESOLVE_DEST_ADDRESS ) );
		return false;
	}

	if ( not destHost.isLocalhost() or inputFields->destManagementAddressPort != 22 )
	{
		external = new NMExternalRemote ( "remote_netserver", true, this );
		NMExternalRemote * nmerNetserver = ( NMExternalRemote * ) external;

		nmerNetserver->setHost ( destHost, inputFields->destManagementAddressPort /*, true*/ );
		nmerNetserver->setPassword ( inputFields->destManagementPassword );
		nmerNetserver->setUsername ( inputFields->destManagementUsername );
	}
	else
		//External Local
		external = new NMExternalLocal ( "local_netserver", true, this );

	*external << inputFields->destNetserverPath;
	*external << "-p" << inputFields->port;
	*external << inputFields->ipVersion;

#ifdef DEBUG
	cout << "***************************************************" << endl;
	cout << "NETSERVER COMMAND: " << external->shellCommand() << endl;
	cout << "***************************************************" << endl;
#endif

#ifdef DEBUG
	cout << LIGHTPURPLE << "(dest)> NMNetperfExternalNetserver::runNetserver" << ENDCOLOR << endl;
#endif
	int error;
	if ( ( error = external->run() ) != NMExternal::NoError )
	{
#ifdef DEBUG
		cerr << RED << "(dest)> [ERROR] Error running Netserver" << ENDCOLOR << endl;
#endif
		if ( error == NMExternal::WrongPassword )
			errorManager->notifyError ( NMError ( NMError::fatal, BAD_PASSWORD_DEST_HOST ) );
		else if ( error == NMExternal::Timeout )
			errorManager->notifyError ( NMError ( NMError::fatal, NETSERVER_SSH_TIMEOUT_ERROR ) );
		else if ( error == NMExternal::Unknow )
			errorManager->notifyError ( NMError ( NMError::fatal, NETSERVER_SSH_UNKNOWN_ERROR ) );
		else 
			errorManager->notifyError ( NMError ( NMError::fatal, NETSERVER_SSH_UNKNOWN_ERROR ) );

		return false;
	}
	statusManager->setStatus ( NMNetperfStatusManager::NetserverStarted );
	external->readyReadOutput.connect ( this, &NMNetperfExternalNetserver::netserverProcessReceived );
	external->finished.connect ( this, &NMNetperfExternalNetserver::netserverProcessExit );
	return true;
}

void NMNetperfExternalNetserver::netserverProcessReceived()
{
#ifdef DEBUG
	cout << LIGHTPURPLE << "(dest)> NMNetperfExternalNetserver::processReceived" << ENDCOLOR << endl;
#endif
	NMString line = external->stdInOut.readLine();
	cout << "\t" << line << endl;

	if ( !isRunning && line.search ( "Starting netserver at" ) >= 0 )
	{
		isRunning = true;
		netserverRunning->netserverRunning();
	}
	else if ( line.search ( "No such file or directory" ) >= 0
	          or line.search ( "No existe el fichero o el directorio" ) >= 0
	          or line.search ( "is a directory" ) >= 0 or line.search ( "es un directorio" ) >= 0
	          or line.search ( "command not found" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, NETSERVER_PATH_WRONG ) );
	else if ( line.search ( "netserver: invalid option" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, NETSERVER_INVALID_OPTION ) );
	else if ( line.search ( "Permission denied" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, NETSERVER_ERROR_PERMISSION ) );
}

void NMNetperfExternalNetserver::netserverProcessExit()
{
#ifdef DEBUG
	cout << LIGHTPURPLE << "(dest)> NMNetperfExternalNetserver::processExit" << ENDCOLOR << endl;
#endif
	finishedNetserver->finishedNetserver();
}

bool NMNetperfExternalNetserver::killNetserver()
{
#ifdef DEBUG
	cout << LIGHTPURPLE << "(dest)> NMNetperfExternalNetserver::killNetserver" << ENDCOLOR << endl;
#endif
	isRunning = false;
	return external->killsignal();
}
