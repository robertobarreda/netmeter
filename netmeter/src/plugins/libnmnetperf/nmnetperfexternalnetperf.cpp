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
#include "nmnetperfexternalnetperf.h"
#include "nmnetperfutils.h"

#include <nmexternallocal.h>
#include <nmexternalremote.h>
#include <nmdns.h>

NMNetperfExternalNetperf::NMNetperfExternalNetperf ( NMParent * parent, NMNetperfErrorManager * errorMan, NMNetperfStatusManager * statusMan, NMNetperfEventFinishedNetperf * endNetperf )
		: NMParent ( "NMNetperfExternalNetperf", parent )
{
	errorManager = errorMan;
	statusManager = statusMan;
	finishedNetperf = endNetperf;
	external = 0;
}

bool NMNetperfExternalNetperf::configureAndRunNetperf ( NMNetperfInputFields * inputFields, const NMString & netperfOutputFilePath )
{
#ifdef DEBUG
	cout << YELLOW << "(source)> NMNetperfExternalNetperf::configureNetperf" << ENDCOLOR << endl;
#endif

	if ( external ) return false;

	statusManager->setStatus ( NMNetperfStatusManager::SendingNetperfScriptFile );

	NMHost sourceHost = NMDns::resolv ( inputFields->sourceManagementAddress,
	                                    ( inputFields->sourceManagementProtocol == "IPv6" ) ? NMDns::IPv6 : NMDns::IPv4 );

	if ( sourceHost.isNull() )
	{
#ifdef DEBUG
		cerr << RED << "[ERROR] Cannot resolve the source address (" << inputFields->sourceManagementAddress << ")." << ENDCOLOR << endl;
#endif
		errorManager->notifyError ( NMError ( NMError::fatal, CANNOT_RESOLVE_SOURCE_ADDRESS ) );
		return false;
	}

	if ( not sourceHost.isLocalhost() or inputFields->sourceManagementAddressPort != 22 )
	{
		external = new NMExternalRemote ( "remote_netperf", true, this );
		NMExternalRemote * nmerNetperf = ( NMExternalRemote * ) external;

		nmerNetperf->setHost ( sourceHost, inputFields->sourceManagementAddressPort /*, true*/ );
		nmerNetperf->setPassword ( inputFields->sourceManagementPassword );
		nmerNetperf->setUsername ( inputFields->sourceManagementUsername );
	}
	else
		//External Local
		external = new NMExternalLocal ( "local_netperf", true, this );

	*external << inputFields->sourceNetperfPath;

	*external << "-H" << inputFields->destAddress;
	*external << "-f k -P 0 -p" << inputFields->port;
	*external << "-l" << inputFields->duration;
	if ( inputFields->udpProtocol ) *external << "-t UDP_STREAM";

	if ( inputFields->noDelayOn or ( inputFields->sms != -1 ) or ( inputFields->sss != -1 )
	        or ( inputFields->rms != -1 ) or ( inputFields->rss != -1 ) )
	{
		*external << "--";
		if ( inputFields->noDelayOn ) *external << "-D";
		if ( inputFields->sms != -1 ) *external << "-m" << inputFields->sms;
		if ( inputFields->sss != -1 ) *external << "-s" << inputFields->sss;
		if ( inputFields->rms != -1 ) *external << "-M" << inputFields->rms;
		if ( inputFields->rss != -1 ) *external << "-S" << inputFields->rss;
	}

	*external << ">" << netperfOutputFilePath;

#ifdef DEBUG
	cout << "*******************************************************************************" << endl;
	cout << "NETPERF COMMAND: " << external->shellCommand() << endl;
	cout << "*******************************************************************************" << endl;
#endif

#ifdef DEBUG
	cout << YELLOW << "(source)> NMNetperfExternalNetperf::runNetperf" << ENDCOLOR << endl;
#endif

	int error;
	if ( ( error = external->run() ) != NMExternal::NoError )
	{
#ifdef DEBUG
		cerr << RED << "Error running Netperf" << ENDCOLOR << endl;
#endif
		if ( error == NMExternal::WrongPassword )
			errorManager->notifyError ( NMError ( NMError::fatal, BAD_PASSWORD_SOURCE_HOST ) );
		else if ( error == NMExternal::Timeout )
			errorManager->notifyError ( NMError ( NMError::fatal, NETPERF_SSH_TIMEOUT_ERROR ) );
		else if ( error == NMExternal::Unknow )
			errorManager->notifyError ( NMError ( NMError::fatal, NETPERF_SSH_UNKNOWN_ERROR ) );
		else
			errorManager->notifyError ( NMError ( NMError::fatal, NETPERF_SSH_UNKNOWN_ERROR ) );

		return false;
	}
	statusManager->setStatus ( NMNetperfStatusManager::NetperfStarted );
	external->readyReadOutput.connect ( this, &NMNetperfExternalNetperf::processReceived );
	external->finished.connect ( this, &NMNetperfExternalNetperf::processExit );
	return true;
}

void NMNetperfExternalNetperf::processReceived()
{
#ifdef DEBUG
	cout << YELLOW << "(source)> NMNetperfExternalNetperf::processReceived" << ENDCOLOR << endl;
#endif
	NMString line = external->stdInOut.readLine();
	cout << "\t" << line << endl;
	
	if ( line.search ( "No such file or directory" ) >= 0
	        or line.search ( "No existe el fichero o el directorio" ) >= 0
	        or line.search ( "is a directory" ) >= 0 or line.search ( "es un directorio" ) >= 0
	        or line.search ( "command not found" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, NETPERF_PATH_WRONG ) );
	else if ( line.search ( "bind() error: Address already in use" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, NETPERF_ADRRESS_ALREADY_IN_USE ) );
	else if ( line.search ( "establish control: are you sure there is a netserver listening" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, NETPERF_ERROR_CONNECT_NETSERVER ) );
	else if ( line.search ( "netperf: invalid option" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, NETPERF_INVALID_OPTION ) );
	else if ( line.search ( "Permission denied" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, NETPERF_ERROR_PERMISSION ) );
	else if ( line.search ( "Usage: netperf  [global options] -- [test options]" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, NETPERF_ERROR_UNKNOWN ) );
}

void NMNetperfExternalNetperf::processExit()
{
#ifdef DEBUG
	cout << YELLOW << "(source)> NMNetperfExternalNetperf::processExit" << ENDCOLOR << endl;
#endif
	if ( statusManager->getStatus() != NMNetperfStatusManager::TestError )
		statusManager->setStatus ( NMNetperfStatusManager::NetperfFinished );
	finishedNetperf->finishedNetperf();
}

void NMNetperfExternalNetperf::killNetperf()
{
#ifdef DEBUG
	cout << YELLOW << "(source)> NMNetperfExternalNetperf::killNetperf" << ENDCOLOR << endl;
#endif
	external->kill();
}
