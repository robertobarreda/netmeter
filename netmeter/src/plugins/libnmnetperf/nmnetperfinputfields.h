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
#ifndef NMNETPERFINPUTFIELDS_H
#define NMNETPERFINPUTFIELDS_H

#include <nmstring.h>

class NMNetperfInputFields
{
	public:
		NMString testName;
		NMString testIdentifier;

		// Source
		NMString sourceAddress;
		NMString sourceNetperfPath;
		NMString sourceManagementAddress;
		int sourceManagementAddressPort;
		NMString sourceManagementUsername;
		NMString sourceManagementPassword;
		NMString sourceManagementProtocol;

		int port;
		int duration;
		bool durationInSeconds; // TRUE if is in Seconds, FALSE if is in Bytes
		bool udpProtocol; // TRUE if protocol = UDP, false if protocol = TCP
		bool noDelayOn; // TRUE if nodelay tag is YES or FALSE if is NO
		int sss, sms, rss, rms;

		// Destination
		NMString destAddress;
		NMString destNetserverPath;
		NMString destManagementAddress;
		int destManagementAddressPort;
		NMString destManagementUsername;
		NMString destManagementPassword;
		NMString destManagementProtocol;

		int ipVersion;
};

#endif
