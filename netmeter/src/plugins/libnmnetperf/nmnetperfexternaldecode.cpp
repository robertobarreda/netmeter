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
#include "nmnetperfexternaldecode.h"
#include "nmnetperfutils.h"

#include <nmdns.h>

NMNetperfExternalDecode::NMNetperfExternalDecode ( NMParent * parent, NMNetperfErrorManager * errorMan,
        NMNetperfStatusManager * statusMan, NMNetperfEventDecodeFinished * decodeEnd )
		: NMParent ( "NMNetperfExternalDecode", parent ), scp ( "decode_scp", true, this )
{
	errorManager = errorMan;
	statusManager = statusMan;
	decodeFinished = decodeEnd;
}

bool NMNetperfExternalDecode::configureAndRunDecode ( NMNetperfInputFields * inputFields, const NMString & netperfOutputFilePath )
{
	#ifdef DEBUG
		cout << "(decode)> NMNetperfExternalDecode::configureDecode" << endl;
	#endif
	statusManager->setStatus ( NMNetperfStatusManager::DecodeStarted );

	NMHost decodeHost = NMDns::resolv ( inputFields->sourceManagementAddress,
	                                    ( inputFields->sourceManagementProtocol == "IPv6" ) ? NMDns::IPv6 : NMDns::IPv4 );

	if ( decodeHost.isNull() )
	{
		#ifdef DEBUG
			cerr << RED << "[ERROR] Cannot resolve the source address (" << inputFields->sourceManagementAddress << ")." << ENDCOLOR << endl;
		#endif
		errorManager->notifyError ( NMError ( NMError::fatal, CANNOT_RESOLVE_SOURCE_ADDRESS ) );
		return false;
	}

	if ( not decodeHost.isLocalhost() or inputFields->sourceManagementAddressPort != 22 )
	{
		NMExternal::Error error;

		statusManager->setStatus ( NMNetperfStatusManager::ReceivingDecodeResultFile );

		scp.setHost ( decodeHost, inputFields->sourceManagementAddressPort );
		scp.setUsernameAndPassword ( inputFields->sourceManagementUsername, inputFields->sourceManagementPassword );
		scp.finished.connect ( this, &NMNetperfExternalDecode::decodeFileSent );

		cout << "(decode)> [INFO] scp get from : " << inputFields->sourceManagementAddress;
		cout << " " << netperfOutputFilePath << "(" << LOCAL_SUFFIX << ")" << endl;

		if ( ( error = scp.get ( netperfOutputFilePath, netperfOutputFilePath + LOCAL_SUFFIX ) ) != NMExternal::NoError )
		{
			#ifdef DEBUG
				cerr << RED << "Error connecting to host " << inputFields->sourceManagementAddress << " with SCP." << ENDCOLOR << endl;
			#endif

			if ( error == NMExternal::WrongPassword )
				errorManager->notifyError ( NMError ( NMError::fatal, BAD_PASSWORD_SOURCE_HOST ) );
			else if ( error == NMExternal::Timeout )
				errorManager->notifyError ( NMError ( NMError::fatal, DECODE_SCP_TIMEOUT_ERROR ) );
			else if ( error == NMExternal::Unknow )
				errorManager->notifyError ( NMError ( NMError::fatal, DECODE_SCP_UNKNOWN_ERROR ) );
			else errorManager->notifyError ( NMError ( NMError::fatal, DECODE_SCP_UNKNOWN_ERROR ) );
			return false;
		}
	}
	else
		decodeFileSent();

	return true;
}

void NMNetperfExternalDecode::decodeFileSent()
{
#ifdef DEBUG
	cout << "(decode)> NMGNetperfExternalDecode::decodeFileSent" << endl;
#endif

	statusManager->setStatus ( NMNetperfStatusManager::TestFinished );
	decodeFinished->decodeFinished();
}
