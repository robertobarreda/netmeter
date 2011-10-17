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
#include "nmgpreferencesconfigurationfile.h"

#include <QDomDocument>
#include <QDomElement>
#include <QFile>

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @return
 */
NMGPreferencesConfigurationFile::NMGPreferencesConfigurationFile()
	: NMGConfigurationFile()
{}

/**
 * Destructor of the class
 * @return
 */
NMGPreferencesConfigurationFile::~NMGPreferencesConfigurationFile()
{}

/**
  * Returns the confiuration file type name
 * @return
 */
QString NMGPreferencesConfigurationFile::getConfigurationFileType()
{
	return "NMGPreferencesConfigurationFile";
}

/**
 * Reads the file from disk
 * @return Returns true if succesful
 */
bool NMGPreferencesConfigurationFile::readFromDisk()
{
	QFile file ( path );
	QDomDocument document;
	if ( !file.open ( QIODevice::ReadOnly ) )
	{
		error = "Error opening file: the file " + path + " doesn't exist or is not readable.";
		return false;
	}
	QString errorMessage;
	int errorLine, errorColumn;
	if ( !document.setContent ( &file, &errorMessage, &errorLine, &errorColumn ) )
	{
		error = "Error: the XML file " + path + " is not well formed (" + errorMessage +
		        " - line " + QString ( "%1" ).arg ( errorLine ) + ", column " + QString ( "%1" ).arg ( errorColumn ) + ").";
		file.close();
		return false;
	}
	file.close();

	QDomElement root = document.documentElement();
	if ( root.tagName() != TAG_PREFERENCES_ROOT )
	{
		error = "Error: the XML file doesn't contain a valid preferences root '" + root.tagName() + "'.";
		return false;
	}
	else
	{
		for ( QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
			if ( node.isElement() )
			{
				QDomElement elem = node.toElement();
				if ( elem.tagName() == TAG_PREFERENCE )
				{
					QString key = elem.attribute ( ATTR_PREFERENCE_KEY );
					QString value = elem.attribute ( ATTR_PREFERENCE_VALUE );
					if ( !key.isNull() and !value.isNull() ) addPreference ( key, value );
				}
				else cerr << "Preferences XML file with unknown tag " << qPrintable ( elem.tagName() ) << "." << endl;
			}
	}
	return true;
}

/**
 * Writes the file to disk
 * @return Returns true if succesful
 */
bool NMGPreferencesConfigurationFile::writeToDisk()
{
	QDomDocument document;
	QDomElement root = document.createElement ( TAG_PREFERENCES_ROOT );
	document.appendChild ( root );

	for ( NMGPreferencesConfigurationFileMapIterator it = map.begin(); it!=map.end(); ++it )
	{
		QString key = it.key();
		QString value = it.value();
		QDomElement pref = document.createElement ( TAG_PREFERENCE );
		pref.setAttribute ( ATTR_PREFERENCE_KEY, key );
		pref.setAttribute ( ATTR_PREFERENCE_VALUE, value );
		root.appendChild ( pref );
	}

	QFile file ( path );
	if ( !file.open ( QIODevice::WriteOnly ) )
	{
		error = "Error opening file: the file " + path + " doesn't exist or write access to it it's not permitted.";
		return false;
	}
	QString fileContent = document.toString ( 4 );
	if ( file.write ( qPrintable( fileContent ), fileContent.length() ) < 0 )
	{
		error = "Error writing file: the write of the content couldn't be performed (call write).";
		file.close();
		return false;
	}
	file.close();
	return true;
}

/**
 * Returns true if the preference identified by key exists
 * @param key Key of the preference
 * @return
 */
bool NMGPreferencesConfigurationFile::existsPreference ( QString key )
{
	NMGPreferencesConfigurationFileMapIterator it = map.find ( key );
	if ( it == map.end() ) return false;
	return true;
}

/**
 * Returns the value of the preference
 * @param key Key of the preference
 * @return The value of the preference or "" if it doesn't exist
 */
QString NMGPreferencesConfigurationFile::getPreference ( QString key )
{
	NMGPreferencesConfigurationFileMapIterator it = map.find ( key );
	if ( it == map.end() ) return "";
	return *it;
}

/**
 * Adds a preference
 * @param key Identifier of the preference
 * @param value Value of the preference
 */
void NMGPreferencesConfigurationFile::addPreference ( QString key, QString value )
{
	map.insert ( key, value );
}

/**
 * Removes a preference
 * @param key Identifier of the preference
 */
void NMGPreferencesConfigurationFile::removePreference ( QString key )
{
	map.remove ( key );
}
