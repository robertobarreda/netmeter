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
#include "nmghostinformation.h"

/**
 * Contructor of the class
 * @return
 */
NMGHostInformation::NMGHostInformation()
{
	hostAlias = "";
	hostAddress = "";
}

/**
 * Contructor of the class
 * @param address
 * @param alias
 * @return
 */
NMGHostInformation::NMGHostInformation ( QString address, QString alias )
{
	hostAddress = address;
	hostAlias = alias;
}

/**
 * Destructor of the class
 * @return
 */
NMGHostInformation::~NMGHostInformation()
{
	//we have to delete all the allocated Fields
	for ( NMGHostInformationMapIterator it = hostMap.begin(); it!= hostMap.end(); ++it )
	{
		NMGHostInformationField * f = it.key();
		delete f;
	}
}

/**
 * Sets the host adress to s
 * @param s Adress of the host
 */
void NMGHostInformation::setHostAdress ( QString s )
{
	hostAddress = s;
}

/**
 * Returns the host adress
 * @return
 */
QString NMGHostInformation::getHostAdress()
{
	return hostAddress;
}

/**
 * Sets the host alias to s
 * @param s
 */
void NMGHostInformation::setHostAlias ( QString s )
{
	hostAlias = s;
}

/**
 * Returns the host alias
 * @return
 */
QString NMGHostInformation::getHostAlias()
{
	return hostAlias;
}

/**
 * Adds a field value to the host (if the field value existed, it's overwritten)
 * @param module Module of the field
 * @param field Name of the field
 * @param value Value of the field
 */
void NMGHostInformation::addFieldValue ( QString module, QString field, QString value )
{
	NMGHostInformationField *f = new NMGHostInformationField();
	f->module = module;
	f->field = field;
	hostMap.insert ( f, value );
}

/**
 * Returns the value of a field
 * @param module Module of the field
 * @param field Name of the field
 * @return
 */
QString NMGHostInformation::getFieldValue ( QString module, QString field )
{
	NMGHostInformationField *f = new NMGHostInformationField();
	f->module = module;
	f->field = field;
	NMGHostInformationMapIterator it = hostMap.find ( f );
	if ( it == hostMap.end() ) return 0;
	else return *it;
}
