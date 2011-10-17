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
#ifndef NMGCONFIGURATIONFILENMGPREFERENCESCONFIGURATIONFILE_H
#define NMGCONFIGURATIONFILENMGPREFERENCESCONFIGURATIONFILE_H

#include "nmgconfigurationfile.h"

#include <QMap>

#define TAG_PREFERENCES_ROOT	"preferences"
#define TAG_PREFERENCE		"preference"
#define ATTR_PREFERENCE_KEY	"key"
#define ATTR_PREFERENCE_VALUE	"value"

typedef QMap<QString,QString> NMGPreferencesConfigurationFileMap;
typedef NMGPreferencesConfigurationFileMap::iterator NMGPreferencesConfigurationFileMapIterator;

class NMGPreferencesConfigurationFile : public NMGConfigurationFile
{
	public:
		NMGPreferencesConfigurationFile();
		~NMGPreferencesConfigurationFile();
		QString getConfigurationFileType();
		bool readFromDisk();
		bool writeToDisk();
		bool existsPreference ( QString key );
		QString getPreference ( QString key );
		void addPreference ( QString key, QString value );
		void removePreference ( QString key );

	private:
		NMGPreferencesConfigurationFileMap map;
};

#endif
