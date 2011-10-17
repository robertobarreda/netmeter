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
#ifndef NMGSTATIONFIELD_H
#define NMGSTATIONFIELD_H

#include <QString>

/**
* Contains a field of a station
*/
class NMGStationField
{
	public:
		NMGStationField();
		NMGStationField ( NMGStationField * fm );
		NMGStationField ( QString mod, QString id, QString val = "" );
		~NMGStationField();
		QString getModule();
		void setModule ( QString mod );
		QString getFieldId();
		void setFieldId ( QString id );
		QString getValue();
		void setValue ( QString val );

		bool operator== ( const NMGStationField & other ) const;
		bool operator< ( const NMGStationField & other ) const;

	private:
		QString module;
		QString fieldId;
		QString value;
};

#endif
