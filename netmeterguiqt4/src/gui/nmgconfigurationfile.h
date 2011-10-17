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

#ifndef NMGCONFIGURATIONFILE_H
#define NMGCONFIGURATIONFILE_H

#include <QString>

class NMGConfigurationFile
{
	public:
		/**
		 * Contructor of the class
		 * @return 
		 */
		NMGConfigurationFile() { error = path = ""; };

		/**
		 * Destructor of the class
		 * @return 
		 */
		virtual ~NMGConfigurationFile() {}

		/**
		 * Returns the Type of the sub-class
		 * @return 
		 */
		virtual QString getConfigurationFileType() = 0;

		/**
		 * Sets the path of the file
		 * @param newPath Path of the file
		 */
		virtual void setPath ( QString newPath ) { path = newPath; }

		/**
		 * Returns the path of the file
		 * @return 
		 */
		virtual QString getPath() { return path; }

		/**
		 * Reads the file form disk
		 * @return 
		 */
		virtual bool readFromDisk() = 0;

		/**
		 * Writes the file to disk
		 * @return 
		 */
		virtual bool writeToDisk() = 0;

		/**
		 * Returns the text associated to an error (if an error has occurred)
		 * @return 
		 */
		QString getError() { return error; }

	protected:
		QString error;
		QString path;
};

#endif
