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
#include "nmgconfigurationfilefactory.h"

/**
 * Contructor of the class
 * @return
 */
NMGConfigurationFileFactory::NMGConfigurationFileFactory()
{}

/**
 * Destructor of the class
 * @return
 */
NMGConfigurationFileFactory::~NMGConfigurationFileFactory()
{}

/**
 * Creates a configuration file
 * @param path Path of the file name
 * @return The created configuration file or 0 if it doesn't exist
 */
NMGConfigurationFile * NMGConfigurationFileFactory::makeConfigurationFile ( QString path )
{
	NMGConfigurationFile * result;
	if ( ( result = makeXMLConfigurationFile ( path ) ) ) return result;
	if ( ( result = makePlainTextConfigurationFile ( path ) ) ) return result;
	return 0;
}

/**
 * Creates an XML configuration file
 * @param path Path of the file name
 * @return The created configuration file or 0 if it doesn't exist
 */
NMGXMLConfigurationFile * NMGConfigurationFileFactory::makeXMLConfigurationFile ( QString path )
{
	NMGXMLConfigurationFile * f = new NMGXMLConfigurationFile();
	f->setPath ( path );
	f->readFromDisk();
	return f;
}

/**
 * Creates a plain text configuration file
 * @param path Path of the file name
 * @return The created configuration file or 0 if it doesn't exist
 */
NMGPlainTextConfigurationFile * NMGConfigurationFileFactory::makePlainTextConfigurationFile ( QString path )
{
	NMGPlainTextConfigurationFile * f = new NMGPlainTextConfigurationFile();
	f->setPath ( path );
	f->readFromDisk();
	return f;
}
