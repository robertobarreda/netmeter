/***************************************************************************
 *   Copyright (C) 2005 by Pau Capella   *
 *   pcapella@ac.upc.edu   *
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
#ifndef NMHOST_H
#define NMHOST_H
#include "nmparent.h"
#include "nmstring.h"

/**
@brief Provide an IP address.
@author Pau Capella
*/
/**
 It stores both IPv4 and IPv6 addresses so you can easily access on any platform.\n
<b> Example to usage </b>\n
\code
	NMHost host;
	host = NMDns::resolv( "www.upc.edu", NMDns::IPv4);
\endcode
*/

class NMHost{

public:

	NMHost();
	NMHost(const NMString &ip);
	NMHost(const NMHost &ip);
	~NMHost();

	NMHost & operator=(const NMHost &);
	bool operator==(const NMHost &) const;

	bool isIPv4() const;
	bool isIPv6() const;
	bool isNull() const;
	bool isLocalhost() const;

	bool setAddress(const NMString& ip);
	NMString getAddress();

private:
	bool checkAddress(const NMString &ip);

	NMString address;	
	bool isipv4;
};

#endif
