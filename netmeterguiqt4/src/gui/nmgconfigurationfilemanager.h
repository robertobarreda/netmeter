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
#ifndef NMGCONFIGURATIONFILEMANAGER_H
#define NMGCONFIGURATIONFILEMANAGER_H

#include "nmgconfigurationfilefactory.h"
#include "nmgconfigurationfile.h"

/**
* Singleton class which manages allthe configuration files
*/
class NMGConfigurationFileManager
{
	private:
		NMGConfigurationFileManager();

	public:
		~NMGConfigurationFileManager();
		static NMGConfigurationFileManager * self();
		NMGConfigurationFile * getConfigurationFile ( QString path );
		NMGXMLConfigurationFile  * getXMLConfigurationFile ( QString path );
		bool existsFile ( QString path );
		bool removeFile ( QString path );

	private:
		static NMGConfigurationFileManager * instance;
		NMGConfigurationFileFactory factory;
};

#endif
