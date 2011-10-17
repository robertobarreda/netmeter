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
#ifndef NMGMODULEFIELDSLIST_H
#define NMGMODULEFIELDSLIST_H

#include <QList>
#include <QString>

struct NMGModuleFieldsInfo
{
	QString fieldName;
	QString fieldNameToShow;
	QString fieldDefaultValue;
};

typedef QList<NMGModuleFieldsInfo *> NMGFieldsInfoList;
typedef NMGFieldsInfoList::iterator NMGFieldsInfoListIterator;

/**
* List with all the fields of a module
*/
class NMGModuleFieldsList
{
	public:
		NMGModuleFieldsList();
		~NMGModuleFieldsList();
		bool existsField( QString fieldName );
		void addField( QString fieldName, QString fieldNameToShow, QString defaultValue = "" );
		QString getDefaultValue( QString fieldName );
		void removeField( QString fieldName );
		const NMGFieldsInfoList& getList();

	private:
		NMGFieldsInfoList fieldsList;
};

#endif
