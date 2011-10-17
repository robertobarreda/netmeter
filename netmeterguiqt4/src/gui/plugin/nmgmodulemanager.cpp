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
#include "nmgmodulemanager.h"
#include <nmgmainwindow.h>
#include <nmstring.h>

#include <iostream>
using namespace std;

NMGModuleManager * NMGModuleManager::instance = 0;

/**
 * Contructor of the class
 * @return
 */
NMGModuleManager::NMGModuleManager ( QObject *parent, const char *name )
	: QObject ( parent ), NMModuleManager ( 0 ), guiModuleFactory ( this )
{
	setObjectName ( name );
	loadingModulesStartup = false;
	nextId = 1;
	connect ( &guiModuleFactory,SIGNAL ( newModuleAdded ( NMModule* ) ), this, SLOT ( addModule ( NMModule* ) ) );
}

/**
 * Destructor of the class
 * @return
 */
NMGModuleManager::~NMGModuleManager()
{}

/**
 * Returns the unique instance of the class (singleton class)
 * @return
 */
NMGModuleManager * NMGModuleManager::self()
{
	if ( !instance ) nmmodulemanager = instance = new NMGModuleManager();
	return instance;
}

/**
 * Adds a module to the manager
 * @param
 */
void NMGModuleManager::addModule ( NMModule * m )
{
	NMGModuleParent * mod = ( NMGModuleParent * ) m;
	QString namePlugin ( mod->getPluginName().c_str() );
	moduleMapByName.insert ( namePlugin,mod );
	QString nameGUI ( mod->getPluginNameGUI().c_str() );
	moduleMapByGUIName.insert ( nameGUI,mod );
	QString nameCore ( mod->getPluginNameCore().c_str() );
	moduleMapByCoreName.insert ( nameCore,mod );

	emit newModuleAdded ( nameGUI,m->getCapabilities().c_str() );
}

/**
 * Returns true if exists a module with the indicated name
 * @param name Name of the module
 * @return
 */
bool NMGModuleManager::existsModule ( QString name )
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
NMModule * NMGModuleManager::getModule ( QString name )
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
 * Returns the module parent associated to the module mod
 * @param mod Module
 * @return
 */
NMGModuleParent * NMGModuleManager::getModuleParent ( NMGModule * mod )
{
	if ( !mod ) return 0;
	for ( QMapModuleManagerIterator it = moduleMapByName.begin(); it!=moduleMapByName.end(); ++it )
	{
		NMGModuleParent * parent = ( NMGModuleParent * ) it.value();
		if ( parent->hasModuleInterface ( ( NMModuleInterface * ) mod ) ) return parent;
	}
	return 0;
}

/**
 * Creates a new interface of the module with the indicated name
 * @param name Name of the module
 * @return Returns the test id associated to the interface created or 0 if there is no module with that name
 */
QString NMGModuleManager::createModuleInterface ( QString name )
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
 * Removes and deletes the interface nmg
 * @param nmg
 */
void NMGModuleManager::deletePluginInterface ( NMGModule * nmg )
{
        communicationManager.endPendingSubscriptions( instanceManager.getTestIdByModule( nmg ) );
	instanceManager.removeTestIdByModule ( nmg );
	NMModule * m = getModule ( nmg->getName() );
	if ( m ) m->removePluginInterface ( nmg );
}

/**
 * Creates a new identifier for the module interface identified by id
 * @param id Test identifier of the module interface
 * @return New test id associated to Module Interface or 0 if there is no such module interface
 */
QString NMGModuleManager::duplicateModuleInterface ( QString id )
{

	NMGModule * m = instanceManager.getModule ( id );
	if ( !m ) return 0;
	QString testId = instanceManager.newTestId();
	instanceManager.addModule ( testId,m );
	m->setTestId ( testId );
	return testId;
}

/**
 * Slot which loads a new module
 * @param path
 */
void NMGModuleManager::loadNewModule ( QString path )
{
	loadModule ( path.toAscii().constData() );
}

/**
 * Slots which unloads an existing module
 * @param moduleName
 */
void NMGModuleManager::unloadExistingModule ( QString moduleName )
{
	NMGModuleParent * m = ( NMGModuleParent * )  getModule ( moduleName );
	if ( m )
	{
		NMGApplicationInformation::getApplicationInformation()->getMainWindow()->unloadAllInstancesOfModule ( moduleName );
		moduleMapByName.remove ( m->getPluginName().c_str() );
		moduleMapByCoreName.remove ( m->getPluginNameCore().c_str() );
		moduleMapByGUIName.remove ( m->getPluginNameGUI().c_str() );
		modulelist.remove ( ( NMModule * ) m );
		delete m;
	}
	else cerr << "There is no module loaded with name " << qPrintable ( moduleName ) << "." << endl;
}

/**
 * Returns the module interface associated to the identifier
 * @param id Identifier of the module interface
 * @return The module interface or 0 if it doesn't exist
 */
NMGModule * NMGModuleManager::getModuleInterface ( QString id )
{
	return instanceManager.getModule ( id );
}

/**
 * Returns a QValueList with all the modules
 * @return
 */
QList<NMModule *> NMGModuleManager::getModuleList()
{
	return moduleMapByName.values();
}

/**
 * Returns the NMGModuleInstanceManager
 * @return
 */
NMGModuleInstanceManager &  NMGModuleManager::getInstanceManager()
{
	return instanceManager;
}

/**
 * Returns the NMGModuleCommunicationManager
 * @return
 */
NMGModuleCommunicationManager & NMGModuleManager::getCommunicationManager()
{
	return communicationManager;
}

/**
 * Slot executed when an error is received
 * @param id id of the test that have to receive the error
 * @param data The part of the doucment with the tags <data>...</data>
 */
void NMGModuleManager::errorReceived ( QString id, int errorType, QString errorMessage )
{
	NMGModule * m = instanceManager.getModule ( id );
	if ( !m ) cerr << "[errorReceived] There is no module test with the id " << qPrintable ( id ) << endl;
	else m->errorReceived ( errorType, errorMessage );
}

/**
 * Slot executed when a status is received
 * @param id id of the test that have to receive the status
 * @param data The part of the doucment with the tags <data>...</data>
 */
void NMGModuleManager::statusReceived ( QString id, QString data )
{
	NMGModule * m = instanceManager.getModule ( id );
	if ( !m ) cerr << "[statusReceived] There is no module test with the id " << qPrintable ( id ) << endl;
	else m->statusReceived ( data );
}

/**
 * Makes the module
 * @param path
 * @return
 */
NMModule * NMGModuleManager::makeModule ( const NMString& path )
{
	return guiModuleFactory.makeModule ( path );
}

/**
 * Sets if the loading of the modules is being made during startup of netemeter gui
 * @param startup
 */
void NMGModuleManager::setLoadingModulesStartup ( bool startup )
{
	loadingModulesStartup = startup;
}

/**
 * Returns if we are during the netmeter gui startup
 * @return
 */
bool NMGModuleManager::getLoadingModulesStartup()
{
	return loadingModulesStartup;
}

#include "nmgmodulemanager.moc"
