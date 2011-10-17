/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Ernest Nieto <ernest.nieto@gmail.com>           *
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
#include "nmgdnsresolver.h"

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
using namespace std;


/**
 * Constructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGDnsResolver::NMGDnsResolver ( QObject *parent, const char *name )
	: QObject ( parent )
{
	setObjectName ( name );
}

/**
 * Destructor of the class
 * @return
 */
NMGDnsResolver::~NMGDnsResolver()
{}


/**
 * Resolves an address with the form of xxx.yyy.upc.edu to 166.56.67.77
 * @param address Address like xxx.yyy.upc.edu (the canonical name)
 * @return Address like 166.56.67.77 ("" if error)
 */
QString NMGDnsResolver::resolveAdress ( QString address, bool isIPv4 )
{
	QString result;
	struct hostent *he;
	if ( isIPv4 )
		he = gethostbyname ( qPrintable( address ) );
	else  //protocol IPv6
		he = gethostbyname2 ( qPrintable( address ), AF_INET6 );

	if ( he != NULL ) result = inet_ntoa ( * ( ( struct in_addr * ) he->h_addr ) );
	else result = "";

	return result;
}

#include "nmgdnsresolver.moc"
