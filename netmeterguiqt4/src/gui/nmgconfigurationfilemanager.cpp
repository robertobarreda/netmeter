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
#include "nmgconfigurationfilemanager.h"
#include "nmgpreferencesmanager.h"

#include <QFile>
#include <QString>

#include <iostream>
using namespace std;


NMGConfigurationFileManager * NMGConfigurationFileManager::instance = 0;

/**
 * Contructor of the class
 * @return
 */
NMGConfigurationFileManager::NMGConfigurationFileManager()
{}

/**
 * Destructor of the class
 * @return
 */
NMGConfigurationFileManager::~NMGConfigurationFileManager()
{}

/**
 * Returns the unique instance of the class (Singleton class)
 * @return
 */
NMGConfigurationFileManager * NMGConfigurationFileManager::self()
{
	if ( !instance ) instance = new NMGConfigurationFileManager();
	return instance;
}

/**
 * Returns a configuration file
 * @param path Path of the file
 * @return Returns a configuration file (0 if it doesn't exists)
 */
NMGConfigurationFile * NMGConfigurationFileManager::getConfigurationFile ( QString path )
{
	if ( path.length() >= 1 and path.at ( 0 ) != '/' )
		path = NMGPreferencesManager::self()->getConfigurationDirectory() + path;
	return factory.makeConfigurationFile ( path );
}

/**
 * Returns an XML configuration file
 * @param path Path of the xml file
 * @return Returns a configuration file (0 if it doesn't exists or it's not an XML)
 */
NMGXMLConfigurationFile  * NMGConfigurationFileManager::getXMLConfigurationFile ( QString path )
{
	if ( path.length() >= 1 and path.at ( 0 ) != '/' )
		path = NMGPreferencesManager::self()->getConfigurationDirectory() + path;
	return factory.makeXMLConfigurationFile ( path );
}

/**
 * Returns true if the indicated file exists
 * @param path Path of the file
 * @return True if the file exists
 */
bool NMGConfigurationFileManager::existsFile ( QString path )
{
	if ( path.length() >= 1 and path.at ( 0 ) != '/' )
		path = NMGPreferencesManager::self()->getConfigurationDirectory() + path;
	return QFile::exists ( path );
}

/**
 * Removes the file if it exists
 * @param path Path of the file
 * @return Returns true if succesful
 */
bool NMGConfigurationFileManager::removeFile ( QString path )
{
	if ( path.length() >= 1 and path.at ( 0 ) != '/' )
		path = NMGPreferencesManager::self()->getConfigurationDirectory() + path;
	QFile file ( path );
	return file.remove();
}
