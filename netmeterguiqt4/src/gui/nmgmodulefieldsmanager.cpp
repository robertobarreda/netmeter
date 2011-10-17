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
#include "nmgmodulefieldsmanager.h"
#include "nmghostinformationmanager.h"

#include <iostream>
using namespace std;

/**
 * Destructor of the class
 * @return
 */
NMGModuleFieldsManager::~NMGModuleFieldsManager()
{
	for ( NMGFieldManagerMapIterator it = fieldMap.begin(); it != fieldMap.end(); ++it )
		delete( it.value() );

	//fieldMap.clear();
}

/**
 * Adds a field to the module
 * @param module Module name
 * @param fieldName Field name
 * @param fieldNameToShow Field name to be shown to the user
 */
void NMGModuleFieldsManager::addField( QString module, QString fieldName, QString fieldNameToShow, QString defaultValue )
{
	NMGFieldManagerMapIterator it = fieldMap.find( module );
	if ( it == fieldMap.end() )
	{
		NMGModuleFieldsList * fl = new NMGModuleFieldsList();
		fl->addField( fieldName, fieldNameToShow, defaultValue );
		fieldMap.insert( module, fl );
	}
	else ( *it )->addField( fieldName, fieldNameToShow, defaultValue );

	NMGHostInformationManager::self()->notifyAddedField( module, fieldName, fieldNameToShow, defaultValue );
}

/**
 * Removes a field of the module
 * @param module Module name
 * @param fieldName Field name
 */
void NMGModuleFieldsManager::removeField( QString module, QString fieldName )
{
	NMGFieldManagerMapIterator it = fieldMap.find( module );
	if ( not( it == fieldMap.end() ) )( *it )->removeField( fieldName );
}

/**
 * Returns of the module contains the specified field
 * @param module Module name
 * @param fieldName Field name
 * @return
 */
bool NMGModuleFieldsManager::existsField( QString module, QString fieldName )
{
	NMGFieldManagerMapIterator it = fieldMap.find( module );
	if ( it == fieldMap.end() ) return false;
	return ( *it )->existsField( fieldName );
}

/**
 * Returns the default value of a field
 * @param module Module name
 * @param fieldName Field name
 * @return The feault value or "" if it doesn't exist
 */
QString NMGModuleFieldsManager::getDefaultValue( QString module, QString fieldName )
{
	NMGFieldManagerMapIterator it = fieldMap.find( module );
	if ( it == fieldMap.end() ) return false;
	return ( *it )->getDefaultValue( fieldName );
}

/**
 * Returns an list with all the fields of a module
 * @param module
 * @return
 */
NMGModuleFieldsList * NMGModuleFieldsManager::getModuleFieldsList( QString module )
{
	NMGFieldManagerMapIterator it = fieldMap.find( module );
	if ( it == fieldMap.end() ) return 0;
	return *it;
}

/**
 * Returns a list with all the module names
 * @return
 */
QStringList NMGModuleFieldsManager::getModuleNames()
{
	return fieldMap.keys();
}
