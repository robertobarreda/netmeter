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
#ifndef NMGSTATIONFIELDSMANAGER_H
#define NMGSTATIONFIELDSMANAGER_H

#include "nmgstationfield.h"

#include <QDomElement>
#include <QDomDocument>
#include <QList>
#include <QSet>
#include <QString>

#define TAG_STATION_FIELDS	"fields"
#define TAG_STATION_FIELD	"field"
#define TAG_STATION_MODULE	"module"
#define TAG_STATION_FIELDID	"id"
#define TAG_STATION_VALUE	"value"

typedef QList<NMGStationField *> NMGStationFieldsManagerList;
typedef NMGStationFieldsManagerList::iterator NMGStationFieldsManagerListIterator;

typedef QSet<QString> QStringSet;
typedef QStringSet::iterator QStringSetIterator;


/**
* Class which manages the fields of a station
*/
class NMGStationFieldsManager
{
	public:
		NMGStationFieldsManager();
		~NMGStationFieldsManager();
		void addField ( QString module, QString fieldId, QString fieldValue );
                void addField ( NMGStationField * s );
		QStringSet getModuleFieldList ();
		QString getField ( QString module, QString fieldId );
                bool containsField ( NMGStationField * s );
		void setFields ( NMGStationFieldsManager & fm );
		void fromXML ( QDomElement element );
		void addFieldNodetoXML ( QDomDocument & doc, QDomElement & parent );

	private:
		NMGStationFieldsManagerList fieldsList;
};

#endif
