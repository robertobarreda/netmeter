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
#include "nmgmoduleinstancemanager.h"

#include <QList>

/**
 * Contructor of the class
 * @param parent QT's parent parameter
 * @param name QT's name parameter
 * @return
 */
NMGModuleInstanceManager::NMGModuleInstanceManager ( QObject *parent, const char *name )
		: QObject ( parent )
{
	setObjectName ( name );
}

/**
 * Destructor of the class
 * @return
 */
NMGModuleInstanceManager::~NMGModuleInstanceManager()
{}

/**
 * Returns true if exists a test with identifier id
 * @param id Identifier of the test
 * @return
 */
bool NMGModuleInstanceManager::existsTest ( QString id )
{
	return instanceMap.contains ( id );
}

/**
 * Returns the module associated to the identifier
 * @param id Test identifier
 * @return Module or 0 if it doesn't exist
 */
NMGModule * NMGModuleInstanceManager::getModule ( QString id )
{
	QMapModuleInstanceManagerIterator it = instanceMap.find ( id );
	if ( it == instanceMap.end() ) return 0;
	return *it;
}

void NMGModuleInstanceManager::addModule ( QString id, NMGModule * m )
{
	instanceMap.insert ( id, m );
}

void NMGModuleInstanceManager::removeModule ( QString id )
{
	instanceMap.remove ( id );
}

/**
 * Returns a new valid test identifier
 * @return A new test identifier
 */
QString NMGModuleInstanceManager::newTestId()
{
	QString id;
	do id.setNum ( random.random() ); while ( existsTest ( id ) );
	return id;
}

/**
 * Returns the NMGModule which has a widget w
 * @param w Widget to search
 * @return NMGModule or 0 if it doesn't exist
 */
NMGModule * NMGModuleInstanceManager::getModuleByWidget ( QWidget * w )
{
	if ( !w ) return 0;
	for ( QMapModuleInstanceManager::const_iterator it = instanceMap.constBegin(); it != instanceMap.constEnd(); ++it )
		if ( it.value() && ( it.value()->getWidget() == w ) ) return it.value();
	return 0;

}

/**
 * Returns the test id of the module
 * @param m Module
 * @return Test id of the module or "" if it there is no instance
 */
QString NMGModuleInstanceManager::getTestIdByModule ( NMGModule * m )
{
	if ( !m ) return "";
	for ( QMapModuleInstanceManager::const_iterator it = instanceMap.constBegin(); it != instanceMap.constEnd(); ++it )
		if ( it.value() && ( it.value() == m ) ) return it.key();
	return "";
}

/**
 * Deletes all the ids associated to the module m
 * @param m Module
 */
void NMGModuleInstanceManager::removeTestIdByModule ( NMGModule * m )
{
	if ( !m ) return;
	QStringList list = instanceMap.keys();
	for ( QStringList::iterator it = list.begin(); it != list.end(); ++it )
		if ( getModule ( *it ) == m ) instanceMap.remove ( *it );
}

/**
 * Removes all the instances  of the manager
 */
void NMGModuleInstanceManager::removeAll()
{
	instanceMap.clear();
}

#include "nmgmoduleinstancemanager.moc"
