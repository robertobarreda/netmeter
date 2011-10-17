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
#include "nmdns.h"
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

NMDns::NMDns()
{
}


NMDns::~NMDns()
{
}

/*! 
    \fn NMDns::resolv(const NMString &name, const NMDnsType &protocol)
    \brief Get network address entry.
    \param name Name to resolv.
    \param protocol IPv4 or IPv6
    \return host object with the IP address.
*/  
NMHost NMDns::resolv(const NMString &name, const NMDnsType &protocol)
{
	NMString address;
	struct hostent *he;
	NMHost host;
	if(protocol == IPv4) 		
		he = gethostbyname(name.c_str());
	else  //protocol IPv6
		he = gethostbyname2(name.c_str(), AF_INET6);

	if(he != NULL) {
		address = inet_ntoa(*((struct in_addr *)he->h_addr));	
		host.setAddress(address);
	}
	else {
		host.setAddress("");
		cout << "Warning, no name host found in dns" << '\n';
	}

	return host;
}




