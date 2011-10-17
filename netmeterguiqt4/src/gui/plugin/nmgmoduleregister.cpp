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
#include "nmgmoduleregister.h"

#include <iostream>
using namespace std;


NMGModuleRegister * NMGModuleRegister::instance = 0;

/**
 * Contructor of the class
 * @return
 */
NMGModuleRegister::NMGModuleRegister ( QObject *parent, const char *name )
	: QObject ( parent )
{
	setObjectName ( name );
	nextId = 1;
}

/**
 * Destructor of the class
 * @return
 */
NMGModuleRegister::~NMGModuleRegister()
{}

/**
 * Returns the unique instance of the class (singleton class)
 * @return
 */
NMGModuleRegister * NMGModuleRegister::self()
{
	if ( !instance ) instance = new NMGModuleRegister();
	return instance;
}

/**
 * Returns true if exists a module with the indicated name
 * @param name Name of the module
 * @return
 */
bool NMGModuleRegister::existsModule ( QString name )
{
	QMapModuleManagerIterator it = moduleMapByName.find ( name );
	if ( not ( it == moduleMapByName.end() ) ) return true;
	it = moduleMapByGUIName.find ( name );
	if ( not ( it == moduleMapByGUIName.end() ) ) return true;
	it = moduleMapByCoreName.find ( name );
	if ( not ( it == moduleMapByCoreName.end() ) ) return true;
	return false;
}

/**
 * Returns the module with the indicated name
 * @param name Name of the module
 * @return Returns the module or 0 if it doesn't exist
 */
NMModule * NMGModuleRegister::getModule ( QString name )
{
	QMapModuleManagerIterator it = moduleMapByName.find ( name );
	if ( not ( it == moduleMapByName.end() ) ) return *it;
	it = moduleMapByGUIName.find ( name );
	if ( not ( it == moduleMapByGUIName.end() ) ) return *it;
	it = moduleMapByCoreName.find ( name );
	if ( not ( it == moduleMapByCoreName.end() ) ) return *it;
	return 0;
}

/**
 * Creates a new interface of the module with the indicated name
 * @param name Name of the module
 * @return Returns the test id associated to the interface created or 0 if there is no module with that name
 */
QString NMGModuleRegister::createModuleInterface ( QString name )
{
	NMModule * m = getModule ( name );
	if ( !m ) return 0;
	unsigned int id = nextId++;
	QString testId = instanceManager.newTestId();
	NMGModule * newInterface = ( NMGModule * ) m->newPluginInterface ( id );
	newInterface->setTestId ( testId );
	instanceManager.addModule ( testId, newInterface );
	return testId;
}

/**
 * Creates a new identifier for the module interface identified by id
 * @param id Test identifier of the module interface
 * @return New test id associated to Module Interface or 0 if there is no such module interface
 */
QString NMGModuleRegister::duplicateModuleInterface ( QString id )
{

	NMGModule * m = instanceManager.getModule ( id );
	if ( !m ) return 0;
	QString testId = instanceManager.newTestId();
	instanceManager.addModule ( testId, m );
	m->setTestId ( testId );
	return testId;
}

/**
 * Returns the module interface associated to the identifier
 * @param id Identifier of the module interface
 * @return The module interface or 0 if it doesn't exist
 */
NMGModule * NMGModuleRegister::getModuleInterface ( QString id )
{
	return instanceManager.getModule ( id );
}

#include "nmgmoduleregister.moc"
