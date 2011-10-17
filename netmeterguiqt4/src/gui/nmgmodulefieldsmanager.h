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
#ifndef NMGMODULEFIELDSMANAGER_H
#define NMGMODULEFIELDSMANAGER_H

#include "nmgmodulefieldslist.h"

#include <QList>
#include <QMap>

typedef QMap<QString, NMGModuleFieldsList *> NMGFieldManagerMap; /* key = module name */
typedef NMGFieldManagerMap::iterator NMGFieldManagerMapIterator;

class NMGModuleFieldsManager
{
	public:
		inline NMGModuleFieldsManager () { fieldMap = NMGFieldManagerMap(); }
		~NMGModuleFieldsManager();
		void addField ( QString module, QString fieldName, QString fieldNameToShow, QString defaultValue = "" );
		void removeField ( QString module, QString fieldName );
		bool existsField ( QString module, QString fieldName );
		QString getDefaultValue ( QString module, QString fieldName );
		NMGModuleFieldsList * getModuleFieldsList ( QString module );
		QStringList getModuleNames();

	private:
		NMGFieldManagerMap fieldMap;
};

#endif
