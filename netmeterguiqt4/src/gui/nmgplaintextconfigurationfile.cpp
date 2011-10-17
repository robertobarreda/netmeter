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
#include "nmgplaintextconfigurationfile.h"

/**
 * Contructor of the class
 * @return 
 */
NMGPlainTextConfigurationFile::NMGPlainTextConfigurationFile()
 : NMGConfigurationFile()
{}

/**
 * Destructor of the class
 * @return 
 */
NMGPlainTextConfigurationFile::~NMGPlainTextConfigurationFile()
{}

/**
 * Returns the confiuration file type name
 * @return 
 */
QString NMGPlainTextConfigurationFile::getConfigurationFileType()
{
	return "NMGPlainTextConfigurationFile";
}


/**
 * Opens the file and creates the XML document
 * @return Returns true if the operation was succesful or false if occurred an error (with the field error with a description)
 */
bool NMGPlainTextConfigurationFile::readFromDisk()
{
	error = "not implemented yet";
	return false;
}

/**
 * Writes the content of the document to disk
 * @return Returns true if the operation was succesful or false if occurred an error (with the field error with a description)
 */
bool NMGPlainTextConfigurationFile::writeToDisk()
{
	error = "not implemented yet";
	return false;
}
