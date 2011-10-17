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
#include "nmgxmlconfigurationfile.h"

#include <QString>
#include <QDomDocument>
#include <QFile>

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @return
 */
NMGXMLConfigurationFile::NMGXMLConfigurationFile()
	: NMGConfigurationFile()
{}

/**
 * Destructor of the class
 * @return
 */
NMGXMLConfigurationFile::~NMGXMLConfigurationFile()
{}

/**
 * Returns the confiuration file type name
 * @return
 */
QString NMGXMLConfigurationFile::getConfigurationFileType()
{
	return "NMGXMLConfigurationFile";
}

/**
 * Opens the file and creates the XML document
 * @return Returns true if the operation was succesful or false if occurred an error (with the field error with a description)
 */
bool NMGXMLConfigurationFile::readFromDisk()
{
	QFile file ( path );
	if ( !file.open ( QIODevice::ReadOnly ) )
	{
		error = "Error opening file: the file " + path + " doesn't exist or is not readable.";
		return false;
	}
	QString errorMessage;
	int errorLine, errorColumn;
	if ( !document.setContent ( &file, & errorMessage, &errorLine, &errorColumn ) )
	{
		error = "Error: the XML file " + path + " is not well formed (" + errorMessage +
		        " - line " + QString ( "%1" ).arg ( errorLine ) + ", column " + QString ( "%1" ).arg ( errorColumn ) + ").";
		file.close();
		return false;
	}
	file.close();
	return true;
}

/**
 * Writes the content of the document to disk
 * @return Returns true if the operation was succesful or false if occurred an error (with the field error with a description)
 */
bool NMGXMLConfigurationFile::writeToDisk()
{
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
 * Returns the XML Document
 * @return QDomDocument
 */
QDomDocument & NMGXMLConfigurationFile::getDomDocument()
{
	return document;
}

