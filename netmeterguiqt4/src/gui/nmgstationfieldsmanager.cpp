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
#include "nmgstationfieldsmanager.h"

#include <QtDebug>
#include <iostream>
using namespace std;

/**
 * Constructor of the class
 * @return
 */
NMGStationFieldsManager::NMGStationFieldsManager()
{
         fieldsList = NMGStationFieldsManagerList();
}

/**
 * Destructor of the class
 * @return
 */
NMGStationFieldsManager::~NMGStationFieldsManager()
{
         fieldsList.clear();
}

/**
 * Adds a field value to the station
 * @param module Module name of the field
 * @param fieldId Field identifier
 * @param fieldValue Field value
 */
void NMGStationFieldsManager::addField ( QString module, QString fieldId, QString fieldValue )
{
	for ( NMGStationFieldsManagerListIterator sf = fieldsList.begin(); sf != fieldsList.end(); ++sf )
	{
		if ( ( *sf )->getModule() == module and ( *sf )->getFieldId() == fieldId )
                {
                        ( *sf )->setValue ( fieldValue );
                        return;
		}
	}
	fieldsList.append ( new NMGStationField ( module, fieldId, fieldValue ) );
}

/**
 * Adds a field value to the station
 * @param s Station field
 */
void NMGStationFieldsManager::addField ( NMGStationField * s )
{
	addField(s->getModule(), s->getFieldId(), s->getValue() );
}

/**
 * Returns a list (QStringSet aka QSet<QString>) with the modules that this field manages.
 * @return List without duplicates (QStringSet aka QSet<QString>) with the module names.
 */
QStringSet NMGStationFieldsManager::getModuleFieldList ()
{
	QStringSet moduleList = QStringSet();
	for ( NMGStationFieldsManagerListIterator sf = fieldsList.begin(); sf != fieldsList.end(); ++sf )
		moduleList.insert(( *sf )->getModule());
	return moduleList;
}

/**
 * Returns the value of a field
 * @param module Module name of the field
 * @param fieldId Field identifier
 * @return Value of the field or "" if it doesn't exists
 */
QString NMGStationFieldsManager::getField ( QString module, QString fieldId )
{
	for ( NMGStationFieldsManagerListIterator sf = fieldsList.begin(); sf != fieldsList.end(); ++sf )
		if ( ( *sf )->getModule() == module and ( *sf )->getFieldId() == fieldId ) 
			return ( *sf )->getValue();
	return QString ();
}

/**
 * Returns TRUE if the field already exists in the list
 * @param s Field that we are looking for
 * @return TRUE if the field exists, FALSE otherwise
 */
bool NMGStationFieldsManager::containsField ( NMGStationField * s )
{
	for ( NMGStationFieldsManagerListIterator sf = fieldsList.begin(); sf != fieldsList.end(); ++sf )
		if ( ( *sf )->getModule() == s->getModule() and ( *sf )->getFieldId() == s->getFieldId() ) return true;
	return false;
}

/**
 * The fields of the field manager will be the same present at fm (they would not be shared)
 * @param fm FiledsManager source to copy the fields from
 */
void NMGStationFieldsManager::setFields ( NMGStationFieldsManager & fm )
{
	for ( NMGStationFieldsManagerListIterator sfm = fm.fieldsList.begin(); sfm != fm.fieldsList.end(); ++sfm )
                addField( *sfm );
}

/**
 * Sets the fields of the field manager which are present in the element
 * @param element QDomElement which contains the tag <fields>
 */
void NMGStationFieldsManager::fromXML ( QDomElement element )
{
	if ( element.tagName() != TAG_STATION_FIELDS )
		cerr << "XML Field element incorrect " << qPrintable ( element.tagName() ) << endl;
	for ( QDomNode node = element.firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			if ( node.nodeName() == TAG_STATION_FIELD )
			{
				QDomElement elem = node.toElement();
				QString module = "", id = "", value = "";
				for ( QDomNode field = elem.firstChild(); !field.isNull(); field = field.nextSibling() )
				{
					if ( field.nodeName() == TAG_STATION_MODULE ) module = field.toElement().text();
					else if ( field.nodeName() == TAG_STATION_FIELDID )  id = field.toElement().text();
					else if ( field.nodeName() == TAG_STATION_VALUE ) value = field.toElement().text();
					else cerr << "Stations field part XML incorrect (" << qPrintable ( field.nodeName() ) << ")." << endl;
				}
				if ( !module.isEmpty() and !id.isEmpty() and !value.isEmpty())
					addField ( module, id, value );
				else
					cerr << "Incomplete field (" << qPrintable ( module ) << "," << qPrintable ( id ) << "," << qPrintable ( value ) << ")." << endl;
			}
			else cerr << "Stations field XML incorrect (" <<  qPrintable ( node.nodeName() ) << ")." << endl;
		}
	}
}

/**
 * Adds the fields to the XML document at the parent elements
 * @param doc XML Document
 * @param parent QDomElement with the tags <station>. The element generated will be added as a child to it
 */
void NMGStationFieldsManager::addFieldNodetoXML ( QDomDocument & doc, QDomElement & parent )
{
	QDomElement rootFields = doc.createElement ( TAG_STATION_FIELDS );
	parent.appendChild ( rootFields );

        //qSort( fieldsList );

	for ( NMGStationFieldsManagerListIterator sf = fieldsList.begin(); sf != fieldsList.end(); ++sf )
	{
		QDomElement field = doc.createElement ( TAG_STATION_FIELD );
		rootFields.appendChild ( field );

		QDomElement module = doc.createElement ( TAG_STATION_MODULE );
		field.appendChild ( module );
		QDomText moduleText = doc.createTextNode ( ( *sf )->getModule() );
		module.appendChild ( moduleText );

		QDomElement id = doc.createElement ( TAG_STATION_FIELDID );
		field.appendChild ( id );
		QDomText idText = doc.createTextNode ( ( *sf )->getFieldId() );
		id.appendChild ( idText );

		QDomElement value = doc.createElement ( TAG_STATION_VALUE );
		field.appendChild ( value );
		QDomText valueText = doc.createTextNode ( ( *sf )->getValue() );
		value.appendChild ( valueText );
	}
}
