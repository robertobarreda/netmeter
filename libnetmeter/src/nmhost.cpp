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
#include "nmhost.h"

/**
\fn NMHost::NMHost()
\brief Creates a new host object with no address.
*/
NMHost::NMHost()
:address(""),isipv4(false)
{
}
 
/**
\fn NMHost::NMHost( const NMString &ip)
\brief Creates a new host object from address ip.
*/
NMHost::NMHost( const NMString &ip)
{
	if(checkAddress(ip))
		address = ip;
}

/**
\fn NMHost::NMHost( const NMHost &ip)
\brief Creates a new host object from host ip
*/
NMHost::NMHost( const NMHost &ip)
{
	address = ip.address;
	isipv4 = ip.isipv4;
}

/**
\fn NMHost::~NMHost()
\brief Destroy host object
*/
NMHost::~NMHost()
{
}

/**
\fn NMHost::checkAddress(const NMString &ip)
\brief Parse the addres.
\return TRUE if is a IPv4 or IPv6 address; otherwise return FALSE;
*/
bool NMHost::checkAddress(const NMString &ip)
{
	//TODO Podriem mirar si els valors son enters.
	//try to ipV4
	int pos = 0, cont = 0;
	NMString nms_address(ip);
	while((pos = nms_address.search(".")) >= 0) {
		nms_address = nms_address.subString(pos+1, nms_address.Length());
		cont ++;
	}
	if(cont == 3) {
		isipv4 = true;
		return true;
	}
	// ok not is a ipv4 try ipv6
	nms_address = ip;
	cont = 0;
	pos = 0;
	while((pos = nms_address.search(":")) >= 0) {
		nms_address = nms_address.subString(pos+1, nms_address.Length());
		cont ++;
	}
	if(pos < 8 && pos > 1) {
		isipv4 = false;
		return true;
	}
	return false;
}

/**
\fn NMHost::isIPv6()
\return Return TRUE if host is a IPv4 protocol.
*/
bool NMHost::isIPv4() const
{
	return isipv4;
}

/**
\fn NMHost::isIPv6()
\return Return TRUE if host is a IPv6 protocol.
*/
bool NMHost::isIPv6() const
{
	return !isipv4;
}

/**
\fn NMHost::isNull()
\return Return TRUE if address is null.
*/
bool NMHost::isNull() const
{
	return !address.Length() ? true : false;
}

/**
\fn NMHost::isLocalhost()
\return Return TRUE if address is localhost.
*/
bool NMHost::isLocalhost() const
{
	return address == "127.0.0.1" || address == "::1" ? true : false;
}

/**
\fn NMHost::operator=( const QNMHost &ip )
\brief Assigns another host address object ip to this object
\param ip Host address
*/
NMHost &NMHost::operator=(const NMHost &ip)
{
	address = ip.address;
	isipv4 = ip.isipv4;
	return *this;	
}

/**
\fn NMHost::operator==( const QNMHost & )
\brief Compare if the host address is the the same that ip
\param ip Host to comapre
\return TRUE if is the same or false, otherwise return FALSE.
*/
bool NMHost::operator==(const NMHost &ip) const
{
	return address == ip.address;
}

/**
\fn NMString NMHost::getAddress()
\return Return the address of host.
*/
NMString NMHost::getAddress()
{
	return address;
}

/**
\fn NMHost::setAddress( const QString& address );
\brief Sets the IPv4 or IPv6 address specified by the string.
\return Return TRUE if the address is parsed correctly, otherwise return FALSE and not set the address.
*/
bool NMHost::setAddress( const NMString& ip )
{
	if(checkAddress(ip)) {
		address = ip;
		return true;
	}
	address = "";
	return false;
}





