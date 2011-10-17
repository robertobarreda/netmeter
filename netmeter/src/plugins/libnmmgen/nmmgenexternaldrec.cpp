/*
 *   $Id: nmmgenexternaldrec.cpp,v 1.21 2007-10-22 09:45:45 jsala Exp $
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
#include "nmmgenexternaldrec.h"
#include "nmmmgenutils.h"
#include <nmdns.h>

/**
 * Contructor of the class
 * @return
 */
NMMGenExternalDrec::NMMGenExternalDrec ( NMParent * parent, NMMgenErrorManager * errorMan,
        NMMgenStatusManager * statusMan, NMMgenEventDrecRunning * drecRun,
        NMMgenEventReadyToRunDrec * readyDrec, NMMgenEventFinishedDrec * finDrec )
		: NMParent ( "NMMGenExternalDrec", parent ), scp ( "drec_scp", true, this )
{
	errorManager = errorMan;
	statusManager = statusMan;
	drecRunning = drecRun;
	readyToRunDrec = readyDrec;
	finishedDrec = finDrec;
	drecExternal = 0;
}

/**
 * Destroyer of the class
 * @return
 */
NMMGenExternalDrec::~NMMGenExternalDrec()
{}

/**
 * Configures the external Drec
 * @param pathDrec Drec path
 * @param pathDecode Decode path
 * @param localPathScript Drec Script Path local
 * @param outputFilePath Drec Output file
 * @param address Adress management of the machine with drec
 * @param username Username management
 * @param password Password management
 * @param protocol Protocol management
 * @param mgenVersion Mgen version in use
 * @return Returns true if all is correct, or false if not (and NMError is send to the GUI module)
 */
bool NMMGenExternalDrec::configureDrec ( const NMString & pathDrec, const NMString & pathDecode, const NMString & localPathScript,
        const NMString & drecOutputFilePath, const NMString & address, int addresPort, const NMString & username, const NMString & password,
        const NMString & protocol, int mgenVersion )
{
	if ( drecExternal ) return false;

	statusManager->setStatus ( NMMgenStatusManager::SendingDrecScriptFile );

	drecPath = pathDrec;
	decodePath = pathDecode;
	managementAddress = address;
	managementAddressPort = addresPort;
	managementPassword = password;
	managementProtocol = protocol == "IPv6" ? "IPv6" : "IPv4";
	managementUsername = username;
	drecOutputFile = drecOutputFilePath;

	if ( managementProtocol == "IPv4" ) destHost = NMDns::resolv ( managementAddress, NMDns::IPv4 );
	else destHost = NMDns::resolv ( managementAddress, NMDns::IPv6 );

	if ( destHost.isNull() )
	{
#ifdef DEBUG
		cerr << RED;
		cerr << "ERROR :: Cannot resolve the source address (" << managementAddress << ")." << endl;
		cerr << ENDCOLOR;
#endif
		errorManager->notifyError ( NMError ( NMError::fatal, CANNOT_RESOLVE_DEST_ADDRESS ) );
		return false;
	}

	if ( not destHost.isLocalhost() or managementAddressPort != 22 )
	{
		isRemote = true;
		NMExternal::Error error;
		scp.setHost ( destHost, managementAddressPort );
		scp.setUsernameAndPassword ( managementUsername, managementPassword );
		scp.finished.connect ( this, &NMMGenExternalDrec::drecFileSent );

		if ( ( error = scp.put ( localPathScript + LOCAL_SUFFIX, localPathScript ) ) != NMExternal::NoError )
		{
#ifdef DEBUG
			cerr << RED;
			cerr << "ERROR :: connecting to host " << managementAddress << " with SCP." << endl;
			cerr << ENDCOLOR;
#endif
			if ( error == NMExternal::WrongPassword )
				errorManager->notifyError ( NMError ( NMError::fatal, BAD_PASSWORD_DEST_HOST ) );
			else if ( error == NMExternal::Timeout )
				errorManager->notifyError ( NMError ( NMError::fatal, DREC_SCP_TIMEOUT_ERROR ) );
			else if ( error == NMExternal::Unknow )
				errorManager->notifyError ( NMError ( NMError::fatal, DREC_SCP_UNKNOWN_ERROR ) );
			else errorManager->notifyError ( NMError ( NMError::fatal, ERROR_CONNECTING_DEST_SCP ) );
			return false;
		}

		drecExternal = new NMExternalRemote ( "remote_drec", true, this );
		( ( NMExternalRemote * ) drecExternal )->setHost ( destHost, managementAddressPort, true );
		( ( NMExternalRemote * ) drecExternal )->setPassword ( managementPassword );
		( ( NMExternalRemote * ) drecExternal )->setUsername ( managementUsername );
	}
	else
	{
		//External Local
		isRemote = false;
		drecExternal = new NMExternalLocal ( "local_drec", true, this );
	}

	*drecExternal << drecPath;
	if ( mgenVersion == 4 ) *drecExternal << "input";
	*drecExternal << localPathScript;
	if ( mgenVersion == 4 ) *drecExternal << "output";
	*drecExternal << drecOutputFile;

	#ifdef DEBUG
		cout << "****************************************************************************************" << endl;
		cout << "DREC COMMAND: " << drecExternal->shellCommand() << endl;
		cout << "****************************************************************************************" << endl;
	#endif

	if ( not isRemote ) readyToRunDrec->readyToRunDrec();

	return true;
}

/**
 * Executed when the drec script file has been sent to the remote station (via scp)
 */
void NMMGenExternalDrec::drecFileSent()
{
	#ifdef DEBUG
		cout << LIGHTPURPLE << "(dest)> NMMGenExternalDrec::drecFileSent" << ENDCOLOR << endl;
	#endif

	if ( not isRemote ) 
		cout << RED << "NMMGenExternalDrec::[panic] Drec file sent executed in a local station!" << ENDCOLOR << endl;
	readyToRunDrec->readyToRunDrec();
}

/**
 * Runs the external drec
 */
bool NMMGenExternalDrec::runDrec()
{
	#ifdef DEBUG
		cout << LIGHTPURPLE << "(dest)> NMMGenExternalDrec::runDrec"<< ENDCOLOR << endl;
	#endif
	if ( !drecExternal ) return false;

	if ( drecExternal->run() != NMExternal::NoError )
	{
	#ifdef DEBUG
		cerr << RED;
		cerr << "ERROR :: running Drec" << endl;
		cerr << ENDCOLOR;
	#endif
		errorManager->notifyError ( NMError ( NMError::fatal, ERROR_RUNNING_DREC ) );
		return false;
	}
	drecExternal->readyReadOutput.connect ( this, &NMMGenExternalDrec::drecProcessReceived );
	drecExternal->finished.connect ( this, &NMMGenExternalDrec::drecProcessExit );
	statusManager->setStatus ( NMMgenStatusManager::DrecStarted );
	return true;
}

/**
 * Executed when the process has received data
 */
void NMMGenExternalDrec::drecProcessReceived()
{
	#ifdef DEBUG
		cout << LIGHTPURPLE << "(dest)> NMMGenExternalDrec::processReceived" << ENDCOLOR << endl;
	#endif
	NMString line = drecExternal->stdInOut.readLine();

	/*#ifdef DEBUG
		cerr << "DEBUG: Drec output" << endl << line << endl << "-- END Drec output" << endl;
	#endif*/

	if ( line.search ( "Hit <CTRL-C> to stop" ) >= 0 )
		drecRunning->drecRunning(); //running.activate();
	else if ( line.search ( "mgen: starting now ..." ) >= 0 )
		//MgenV4 is listening
		drecRunning->drecRunning(); //running.activate();
	else if ( line.search ( "No such file or directory" ) >= 0 or line.search ( "No existe el fichero o el directorio" ) >= 0
	          or line.search ( "is a directory" ) >= 0 or line.search ( "es un directorio" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, DREC_PATH_WRONG ) );
	else if ( line.search ( "Too many arguments" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, MGEN_ERROR_VERSION ) );
}

/**
 * Executed when the process exits
 */
void NMMGenExternalDrec::drecProcessExit()
{
	#ifdef DEBUG
		cout << LIGHTPURPLE << "(dest)> NMMGenExternalDrec::processExit" << ENDCOLOR << endl;
	#endif
	finishedDrec->finishedDrec();
}


/**
 * Kills the drec process
 */
void NMMGenExternalDrec::killDrec()
{
	#ifdef DEBUG
		cout << LIGHTPURPLE << "(dest)> NMMGenExternalDrec::killDrecAndStartDecode" << ENDCOLOR << endl;
	#endif

	//bool killed = drecExternal->killsignal();
	bool killed = drecExternal->termsignal();

	#ifdef DEBUG
		if ( killed ) 
			cout << LIGHTPURPLE << "(dest)> OK :: NMMGenExternalDrec::drec has been killed!" << ENDCOLOR <<endl;
		else 
			cout << RED << "ERROR :: NMMGenExternalDrec::drec has NOT been killed!" << ENDCOLOR <<endl;
	#endif

}
