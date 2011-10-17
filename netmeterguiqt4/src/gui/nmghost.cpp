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
#include "nmghost.h"
#include "nmgdnsresolver.h"

#include <QAbstractSocket>
#include <QHostAddress>

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @return
 */
NMGHost::NMGHost()
{
	address = alias = QString();
	isIPv4 = false;
}

/**
 * Destructor of the class
 * @return
 */
NMGHost::~NMGHost()
{}

/**
 * Returns the address of the host
 * @return
 */
QString NMGHost::getAddress()
{
	return address;
}

/**
 * resolves the address if it was in the canonical form
 * @return
 */
QString NMGHost::resolveAddress()
{
	return NMGDnsResolver::resolveAdress ( address, isHostIPv4() );
}

/**
 * Sets the address of the host
 * @param addr
 */
void NMGHost::setAddress ( QString addr )
{
	address = addr;
	QHostAddress ha( addr );
	setHostIPV4 ( ha.protocol() == QAbstractSocket::IPv4Protocol );
}

/**
 * Returns the alias of the host
 * @return
 */
QString NMGHost::getAlias()
{
	return alias;
}

/**
 * Sets the alias of the host
 * @param newAlias
 * @return
 */
void NMGHost::setAlias ( QString newAlias )
{
	alias = newAlias;
}

/**
 * Returns true if the host is IPv4 or false if the host is IPv6
 * @return
 */
bool NMGHost::isHostIPv4()
{
	return isIPv4;
}

/**
 * Sets the host protocol: True -> host is IPv4, False -> host is IPv6
 * @param ipv4
 */
void NMGHost::setHostIPV4 ( bool ipv4 )
{
	isIPv4 = ipv4;
}
