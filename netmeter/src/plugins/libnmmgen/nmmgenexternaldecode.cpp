/*
 *   $Id: nmmgenexternaldecode.cpp,v 1.15 2007-10-22 09:45:45 jsala Exp $
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
#include "nmmgenexternaldecode.h"
#include "nmmmgenutils.h"
#include <nmdns.h>

/**
 * Contructor of the class
 * @return
 */
NMMGenExternalDecode::NMMGenExternalDecode ( NMParent * parent, NMMgenErrorManager * errorMan,
        NMMgenStatusManager * statusMan, NMMgenEventDecodeFinished * decodeEnd )
		: NMParent ( "NMMGenExternalDecode", parent ), scp ( "decode_scp", true, this )
{
	errorManager = errorMan;
	statusManager = statusMan;
	decodeFinished = decodeEnd;
	decodeExternal = 0;
}

/**
 * Destroyer of the class
 * @return
 */
NMMGenExternalDecode::~NMMGenExternalDecode()
{}

/**
 * Configures the decode
 * @param pathDecode
 * @param drecOutputFilePath
 * @param decodeOutputFilePath
 * @param address Adress management of the machine with decode
 * @param username Username management
 * @param password Password management
 * @param protocol Protocol management
 * @param mgenVersion Mgen version in use
 * @return Returns true if all OK, false otherwise
 */
bool NMMGenExternalDecode::configureDecode ( const NMString & pathDecode, const NMString & drecOutputFilePath,
        const NMString & decodeOutputFilePath, const NMString & address, int addressPort, const NMString & username,
        const NMString & password, const NMString & protocol, int mgenVersion )
{
	if ( decodeExternal ) return false;

	decodePath = pathDecode;
	managementAddress = address;
	managementAddressPort = addressPort;
	managementUsername = username;
	managementPassword = password;
	managementProtocol = protocol == "IPv6" ? "IPv6" : "IPv4";
	drecOutputFile = drecOutputFilePath;
	decodeOutputFile = decodeOutputFilePath;

	if ( managementProtocol == "IPv4" ) destHost = NMDns::resolv ( managementAddress, NMDns::IPv4 );
	else destHost = NMDns::resolv ( managementAddress, NMDns::IPv6 );

	if ( destHost.isNull() )
	{
#ifdef DEBUG
		cerr << RED;
		cerr << "ERROR :: NMMGenExternalDecode cannot resolve the source address (" << managementAddress << ")." << endl;
		cerr << ENDCOLOR;
#endif
		errorManager->notifyError ( NMError ( NMError::fatal, CANNOT_RESOLVE_DEST_ADDRESS ) );
		return false;
	}


	if ( not destHost.isLocalhost() or managementAddressPort != 22 )
	{
		//External Remote
		isRemote = true;
		decodeExternal = new NMExternalRemote ( "remote_decode", true, this );
		( ( NMExternalRemote * ) decodeExternal )->setHost ( destHost, managementAddressPort, true );
		( ( NMExternalRemote * ) decodeExternal )->setPassword ( managementPassword );
		( ( NMExternalRemote * ) decodeExternal )->setUsername ( managementUsername );
	}
	else
	{
		isRemote = false;
		decodeExternal = new NMExternalLocal ( "local_decode", true, this );
	}

	switch ( mgenVersion )
	{
		case 3:
			*decodeExternal << decodePath;
			*decodeExternal << drecOutputFile;
			*decodeExternal << ">";
			*decodeExternal << decodeOutputFile;
			break;
		case 4:
			*decodeExternal << "mv";
			*decodeExternal << drecOutputFile;
			*decodeExternal << decodeOutputFile;
			break;

	}

#ifdef DEBUG
	cout << "****************************************************************************************" << endl;
	cout << "DECODE COMMAND: " << decodeExternal->shellCommand() << endl;
	cout << "****************************************************************************************" << endl;
#endif

	return true;
}

/**
 * Starts the execution of decode
 */
bool  NMMGenExternalDecode::startDecode()
{
	#ifdef DEBUG
		cout << LIGHTPURPLE << "(dest)> NMMGenExternalDecode::startDecode" << ENDCOLOR << endl;
	#endif
	if ( decodeExternal->run() != NMExternal::NoError )
	{
	#ifdef DEBUG
		cerr << RED << "ERROR:: NMMGenExternalDecode error running decode." << ENDCOLOR << endl;
	#endif
		errorManager->notifyError ( NMError ( NMError::fatal, ERROR_RUNNING_DECODE ) );
		return false;
	}
	decodeExternal->finished.connect ( this, &NMMGenExternalDecode::decodeProcessExit );
	decodeExternal->readyReadOutput.connect ( this, &NMMGenExternalDecode::decodeProcessReceived );
	statusManager->setStatus ( NMMgenStatusManager::DecodeStarted );
	return true;
}

/**
 * Executed when the decode process has incoming data to read
 */
void NMMGenExternalDecode::decodeProcessReceived()
{
	#ifdef DEBUG
		cout << LIGHTPURPLE << "(dest)> NMMGenExternalDecode::decodeProcessReceived" << ENDCOLOR << endl;
	#endif

	NMString line = decodeExternal->stdInOut.readLine();
	/*
		#ifdef DEBUG
		  cerr << "DEBUG: Decode output" << endl;
		  cerr << line << endl;
		  cerr << "SHELL COMMAND: " << decodeExternal->shellCommand() << endl;
		#endif
	*/
	if ( line.search ( "No such file or directory" ) >= 0 or line.search ( "No existe el fichero o el directorio" ) >= 0
	        or line.search ( "is a directory" ) >= 0 or line.search ( "es un directorio" ) >= 0 )
		errorManager->notifyError ( NMError ( NMError::fatal, DECODE_PATH_WRONG ) );
}

/**
 * Executed when the decode process exits
 */
void NMMGenExternalDecode::decodeProcessExit()
{
	#ifdef DEBUG
		cout << LIGHTPURPLE << "(dest)> NMGMGenExternalDecode::decodeProcessExit" << ENDCOLOR << endl;
	#endif

	if ( isRemote )
	{
		NMExternal::Error error;

		statusManager->setStatus ( NMMgenStatusManager::ReceivingDecodeResultFile );

		scp.setHost ( destHost, managementAddressPort );
		scp.setUsernameAndPassword ( managementUsername, managementPassword );
		scp.finished.connect ( this, &NMMGenExternalDecode::decodeFileSent );
		if ( ( error = scp.get ( decodeOutputFile, decodeOutputFile + LOCAL_SUFFIX ) ) != NMExternal::NoError )
		{
		#ifdef DEBUG
			cerr << RED;
			cerr << "ERROR :: MMGenExternalDecode while connecting via scp to host to get the remote output decode file" << endl;
			cerr << ENDCOLOR;
		#endif
			if ( error == NMExternal::WrongPassword )
				errorManager->notifyError ( NMError ( NMError::fatal, BAD_PASSWORD_DEST_HOST ) );
			else if ( error == NMExternal::Timeout )
				errorManager->notifyError ( NMError ( NMError::fatal, DECODE_SCP_TIMEOUT_ERROR ) );
			else if ( error == NMExternal::Unknow )
				errorManager->notifyError ( NMError ( NMError::fatal, DECODE_SCP_UNKNOWN_ERROR ) );
			else errorManager->notifyError ( NMError ( NMError::fatal, ERROR_CONNECTING_DEST_SCP ) );
			return;
		}
	}
	else
	{
		statusManager->setStatus ( NMMgenStatusManager::TestFinished );
		decodeFinished->decodeFinished();
	}
}

/**
 * Executed when the decode output file has been sent to local (via scp)
 */
void NMMGenExternalDecode::decodeFileSent()
{
	#ifdef DEBUG
		cout << LIGHTPURPLE << "(dest)> NMGMGenExternalDecode::decodeFileSent" << ENDCOLOR << endl;
	#endif

	statusManager->setStatus ( NMMgenStatusManager::TestFinished );

	decodeFinished->decodeFinished();
}

/**
 * Kills the external decode
 */
void NMMGenExternalDecode::killDecode()
{
	#ifdef DEBUG
		cout << LIGHTPURPLE << "(dest)> NMMGenExternalDecode::killDecode" << ENDCOLOR << endl;
	#endif
	decodeExternal->termsignal();
}
