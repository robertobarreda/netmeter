/*
 *   $Id: nmmgenexternalmgen.cpp,v 1.21 2007-10-22 09:45:45 jsala Exp $
 *
 *   Copyright (c) 2006 Ernest Nieto
 *   Copyright (c) 2008 Roberto Barreda
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
#include "nmmgenexternalmgen.h"
#include "nmmmgenutils.h"
#include <nmdns.h>

/**
 * Constructor of the class
 * @return
 */
NMMGenExternalMgen::NMMGenExternalMgen ( NMParent * parent, NMMgenErrorManager * errorMan, NMMgenStatusManager * statusMan,
        NMMgenEventFinishedMgen * endMgen, NMMgenEventReadyToRunMgen * runMgen )
		: NMParent ( "NMMGenExternalMgen", parent ), scp ( "mgen_scp", true, this )
{
	errorManager = errorMan;
	statusManager = statusMan;
	finisehdMgen = endMgen;
	readyToRunMgen = runMgen;
	external = 0;
}

/**
 * Destroyer of the class
 * @return
 */
NMMGenExternalMgen::~NMMGenExternalMgen()
{}

/**
 * Configures the external Mgen
 * @param pathMgen Path of Mgen
 * @param localPathScript Local path of the file with the script for mgen
 * @param address Management address of the station
 * @param username Username management
 * @param password Password management
 * @param protocol Protocol management
 * @param mgenVersion Mgen version in use
 * @return True if sucessful
 */
bool NMMGenExternalMgen::configureMgen ( const NMString & pathMgen, const NMString & localPathScript, 
	const NMString & address, int addressPort, const NMString & username, const NMString & password, 
	const NMString & protocol, int mgenVersion )
{
	#ifdef DEBUG
		cout << YELLOW << "(source)> NMMGenExternalMgen::configureMgen" << ENDCOLOR <<endl;
	#endif

	if ( external ) return false;

	statusManager->setStatus ( NMMgenStatusManager::SendingMgenScriptFile );

	mgenPath = pathMgen;
	managementAddress = address;
	managementAddressPort = addressPort;
	managementPassword = password;
	managementProtocol = protocol == "IPv6" ? "IPv6" : "IPv4";
	managementUsername = username;

	NMHost sourceHost;
	if ( managementProtocol == "IPv4" ) sourceHost = NMDns::resolv ( managementAddress, NMDns::IPv4 );
	else sourceHost = NMDns::resolv ( managementAddress, NMDns::IPv6 );

	if ( sourceHost.isNull() )
	{
		#ifdef DEBUG
			cerr << RED;
			cerr << "ERROR :: Cannot resolve the source address (" << managementAddress << ")." << endl;
			cerr << ENDCOLOR;
		#endif
		errorManager->notifyError ( NMError ( NMError::fatal, CANNOT_RESOLVE_SOURCE_ADDRESS ) );
		return false;
	}

	if ( not sourceHost.isLocalhost() or managementAddressPort != 22 )
	{
		isRemote = true;
		NMExternal::Error error;
		scp.setHost ( sourceHost, managementAddressPort );
		scp.setUsernameAndPassword ( managementUsername, managementPassword );
		scp.finished.connect ( this, &NMMGenExternalMgen::mgenScriptSent );

		if ( ( error = scp.put ( localPathScript + LOCAL_SUFFIX, localPathScript ) ) != NMExternal::NoError )
		{
			#ifdef DEBUG
				cerr << RED;
				cerr << "ERROR :: connecting to host " << managementAddress << " with SCP." << endl;
				cerr << ENDCOLOR;
			#endif

			if ( error == NMExternal::WrongPassword )
				errorManager->notifyError ( NMError ( NMError::fatal, BAD_PASSWORD_SOURCE_HOST ) );
			else if ( error == NMExternal::Timeout )
				errorManager->notifyError ( NMError ( NMError::fatal, MGEN_SCP_TIMEOUT_ERROR ) );
			else if ( error == NMExternal::Unknow )
				errorManager->notifyError ( NMError ( NMError::fatal, MGEN_SCP_UNKNOWN_ERROR ) );
			else errorManager->notifyError ( NMError ( NMError::fatal, ERROR_CONNECTING_SOURCE_SCP ) );
			return false;
		}


		external = new NMExternalRemote ( "remote_mgen", true, this );
		NMExternalRemote *nmerMgen = ( NMExternalRemote * ) external;

		nmerMgen->setHost ( sourceHost, addressPort, true );
		nmerMgen->setPassword ( managementPassword );
		nmerMgen->setUsername ( managementUsername );
	}
	else
	{
		//External Local
		isRemote = false;
		external = new NMExternalLocal ( "local_mgen", true, this );
	}

	*external << mgenPath;
	if ( mgenVersion == 4 ) *external << "input";
	*external << localPathScript;

	#ifdef DEBUG
		cout << "****************************************************************************************" << endl;
		cout << "MGEN COMMAND: " << external->shellCommand() << endl;
		cout << "****************************************************************************************" << endl;
	#endif

	if ( not isRemote ) readyToRunMgen->readyToRunMgen();
	return true;
}

/**
 * Executed when the mgen script has been sent to the remote host
 */
void NMMGenExternalMgen::mgenScriptSent()
{
	#ifdef DEBUG
		cout << YELLOW << "(source)> NMMGenExternalMgen::mgenScriptSent" << ENDCOLOR << endl;
	#endif

	if ( not isRemote ) 	cout << RED << "NMMGenExternalMgen::[panic] Mgen file sent executed in a local station!" << ENDCOLOR << endl;
	readyToRunMgen->readyToRunMgen();
}

/**
 * Runs mgen
 */
bool NMMGenExternalMgen::runMgen()
{
	#ifdef DEBUG
		cout << YELLOW << "(source)> NMMGenExternalMgen::runMgen" << ENDCOLOR << endl;
	#endif

	if ( !external ) return false;
	if ( external->run() != NMExternal::NoError )
	{
		errorManager->notifyError ( NMError ( NMError::fatal, ERROR_RUNNING_MGEN ) );
		return false;
	}
	external->readyReadOutput.connect ( this, &NMMGenExternalMgen::processReceived );
	external->finished.connect ( this, &NMMGenExternalMgen::processExit );
	statusManager->setStatus ( NMMgenStatusManager::MgenStarted );
	return true;
}

/**
 * Method executed when there is data from the mgen process
 */
void NMMGenExternalMgen::processReceived()
{
	NMString line = external->stdInOut.readLine();

	#ifdef DEBUG
		//cout << "DEBUG: Mgen Output" << endl << line << endl;
		cout << YELLOW << "(source)> NMMGenExternalMgen::processReceived" << ENDCOLOR << endl;
	#endif

	if ( line.search ( "No such file or directory" ) >= 0 or line.search ( "No existe el fichero o el directorio" ) >= 0
	        or line.search ( "is a directory" ) >= 0 or line.search ( "es un directorio" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, MGEN_PATH_WRONG ) );
	else if ( line.search ( "bind() error: Address already in use" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, MGEN_ADRRESS_ALREADY_IN_USE ) );
	else if ( line.search ( "Error parsing script" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, MGEN_ERROR_PARSING_SCRIPT ) );
	else if ( line.search ( "Error loading script" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, MGEN_ERROR_LOADING_SCRIPT ) );
	else if ( line.search ( "MGEN: Error " ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, MGEN_ERROR_UNKNOWN ) );
}

/**
 * Method executed when the execution of mgen ends
 */
void NMMGenExternalMgen::processExit()
{
	#ifdef DEBUG
		cout << YELLOW << "(source)> NMMGenExternalMgen::processExit" << ENDCOLOR << endl;
	#endif

	statusManager->setStatus ( NMMgenStatusManager::MgenFinished );
	finisehdMgen->finishedMgen();
}

/**
 * Kills the external mgen
 */
void NMMGenExternalMgen::killMgen()
{
	#ifdef DEBUG
		cout << YELLOW << "(source)> NMMGenExternalMgen::killMgen" << ENDCOLOR << endl;
	#endif
	
	//external->killsignal();
	external->termsignal();
}
