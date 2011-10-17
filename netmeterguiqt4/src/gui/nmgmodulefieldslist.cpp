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
#include "nmgmodulefieldslist.h"

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @return
 */
NMGModuleFieldsList::NMGModuleFieldsList()
{}

/**
 * Destructor of the class
 * @return
 */
NMGModuleFieldsList::~NMGModuleFieldsList()
{}

/**
 * Returns true if the fiel exists
 * @param fieldName
 * @return
 */
bool NMGModuleFieldsList::existsField ( QString fieldName )
{
	for ( NMGFieldsInfoListIterator it = fieldsList.begin(); it != fieldsList.end(); ++it )
		if ( ( *it )->fieldName == fieldName ) return true;
	return false;
}

/**
 * Adds a field to the list (it does'nt check if the field exists)
 * @param fieldName Field name
 * @param fieldNameToShow Field name to bew shown to the user
 */
void NMGModuleFieldsList::addField ( QString fieldName, QString fieldNameToShow, QString defaultValue )
{
	if ( existsField ( fieldName ) ) return;
	NMGModuleFieldsInfo * fi = new NMGModuleFieldsInfo();
	fi->fieldName = fieldName;
	fi->fieldNameToShow  = fieldNameToShow;
	fi->fieldDefaultValue = defaultValue;
	fieldsList.append ( fi );
}

/**
 * Returns the default value of a field or "" if it doesn't exist
 * @param fieldName Field name
 * @return
 */
QString NMGModuleFieldsList::getDefaultValue ( QString fieldName )
{
	for ( NMGFieldsInfoListIterator it = fieldsList.begin(); it != fieldsList.end(); ++it )
		if ( ( *it )->fieldName == fieldName ) return ( *it )->fieldDefaultValue;
	return "";
}

/**
 * Removes a field from the list
 * @param fieldName Name of the field to remove
 */
void NMGModuleFieldsList::removeField ( QString fieldName )
{
	for ( NMGFieldsInfoListIterator it = fieldsList.begin(); it != fieldsList.end(); ++it )
		if ( ( *it )->fieldName == fieldName ) fieldsList.erase ( it );
}

/**
 * Returns the list
 * @return
 */
const NMGFieldsInfoList & NMGModuleFieldsList::getList()
{
	return fieldsList;
}
