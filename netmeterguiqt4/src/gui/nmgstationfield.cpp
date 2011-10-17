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
#include "nmgstationfield.h"

/**
 * Empty contructor of the class
 * @return
 */
NMGStationField::NMGStationField()
{
	module = fieldId = value = QString();
}

/**
 * Contructor of the class
 * @param fm StationField to copy the fields
 * @return
 */
NMGStationField::NMGStationField ( NMGStationField * fm )
{
	module = fm->getModule();
	fieldId = fm->getFieldId();
	value = fm->getValue();
}

/**
 * Contructor of the class
 * @param mod
 * @param id
 * @param val
 * @return
 */
NMGStationField::NMGStationField ( QString mod, QString id, QString val )
		: module ( mod ), fieldId ( id ), value ( val )
{}

/**
 * Destructor of the class
 * @return
 */
NMGStationField::~NMGStationField()
{}


/**
 * Returns the moduel name of the field
 * @return
 */
QString NMGStationField::getModule()
{
	return module;
}

/**
 * Sets the module name of the field
 * @param mod
 */
void NMGStationField::setModule ( QString mod )
{
	module = mod;
}

/**
 * Returns the field identifier of the field
 * @return
 */
QString NMGStationField::getFieldId()
{
	return fieldId;
}

/**
 * Sets the identifier of the field
 * @param id
 */
void NMGStationField::setFieldId ( QString id )
{
	fieldId = id;
}

/**
 * Returns the value of the field
 * @return
 */
QString NMGStationField::getValue()
{
	return value;
}

/**
 * Sets the value of the field
 * @param val
 */
void NMGStationField::setValue ( QString val )
{
	value = val;
}

bool NMGStationField::operator== ( const NMGStationField & other ) const
{
	NMGStationField * elem1 = ( NMGStationField * ) & other;
	NMGStationField * elem2 = ( NMGStationField * ) this;

	return ( bool ) ( ( elem1->getModule() == elem2->getModule() )
	                  && ( elem1->getFieldId() == elem2->getFieldId() )
	                  && ( elem1->getValue() == elem2->getValue() ) );
}

bool NMGStationField::operator< ( const NMGStationField & other ) const
{
	NMGStationField * elem1 = ( NMGStationField * ) this;
	NMGStationField * elem2 = ( NMGStationField * ) & other;

	if ( elem1->getModule() < elem2->getModule() ) return true;
	else if ( elem1->getModule() > elem2->getModule() ) return false;
	else
	{
		if ( elem1->getFieldId() < elem2->getFieldId() ) return true;
		else if ( elem1->getFieldId() > elem2->getFieldId() ) return false;
		else
		{
			// Just if it's necessary, in the future, to order also by value
			return ( bool ) ( elem1->getValue() <= elem2->getValue() );
		}
	}
}
