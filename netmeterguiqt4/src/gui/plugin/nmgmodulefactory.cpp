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
#include "nmgmodulefactory.h"

#include "nmgmodulemanager.h"
#include "nmgmoduleparent.h"

#include <nmgpreferencesmanager.h>

#include <dlfcn.h>

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @param m
 * @param parent
 * @param name
 * @return
 */
NMGModuleFactory::NMGModuleFactory ( NMParent *m, QObject *parent, const char *name )
	: QObject ( parent ), NMModuleFactory ( m )
{
	setObjectName ( name );
}

/**
 * Destructor of the class
 * @return
 */
NMGModuleFactory::~NMGModuleFactory()
{}

/**
 * Creates a new module
 * @param name
 * @param parent
 * @return
 */
NMModule* NMGModuleFactory::createNewModule ( NMString name, NMParent * parent )
{
	return new NMGModuleParent ( name, parent );
}


/**
 * Creates a module and check if it doesn't exist
 * @param nms_path
 * @return
 */
NMModule* NMGModuleFactory::makeModule ( const NMString nms_path )
{
	NMGModuleParent * m = ( NMGModuleParent * ) NMModuleFactory::makeModule ( nms_path );

	if ( !m ) return 0;
	//check if the module wants to be loaded
	if ( NMGModuleManager::self()->getLoadingModulesStartup() )
	{
		QString modulesToLoad = NMGPreferencesManager::self()->getValue ( ID_MODULES_TO_LOAD );
		bool loadAllModules = NMGPreferencesManager::self()->getValue ( ID_LOAD_ALL_MODULES ) !=ID_LOAD_ALL_MODULES_FALSE;
		if ( not loadAllModules )
		{
			if ( not ( ( not QString ( m->getPluginName().c_str() ).isEmpty() 
					and modulesToLoad.contains ( m->getPluginName().c_str() ) ) or
			           ( not QString ( m->getPluginNameCore().c_str() ).isEmpty() 
					and modulesToLoad.contains ( m->getPluginNameCore().c_str() ) ) or
			           ( not QString ( m->getPluginNameGUI().c_str() ).isEmpty() 
					and modulesToLoad.contains ( m->getPluginNameGUI().c_str() ) ) ) )
			{
				NMGModuleManager::self()->unloadExistingModule ( m->getPluginName().c_str() );
				return 0;
			}
		}
	}

	if ( NMGModuleManager::self()->getModule ( m->getPluginName().c_str() ) or
	        NMGModuleManager::self()->getModule ( m->getPluginName().c_str() ) or
	        NMGModuleManager::self()->getModule ( m->getPluginName().c_str() ) )
	{
		cerr << "The module " << m->getPluginName() << "(" << nms_path.c_str() << ") is allready loaded." << endl;
		delete m;
		return NULL;
	}

	emit newModuleAdded ( m );
	return m;
}

/**
 * Makes the work necessary to create the GUI plugins
 * @param handle
 * @param m
 */
void NMGModuleFactory::makeModulePlantilla ( void *handle, NMModule* m )
{
	NMModuleFactory::makeModulePlantilla ( handle,m );
	NMGModuleParent * mod = ( NMGModuleParent * ) m;

	NMMODULE_TEXT nmmnameGUI;
	NMMODULE_TEXT nmmnameCore;
	NMMODULE_TEXT nmmIcon;
	NMMODULE_TEXT nmmToolTip;

	nmmnameGUI = ( NMMODULE_TEXT ) dlsym ( handle, "getModuleGUIName" );
	nmmnameCore = ( NMMODULE_TEXT ) dlsym ( handle, "getModuleCoreName" );
	nmmIcon = ( NMMODULE_TEXT ) dlsym ( handle, "getModuleIconName" );
	nmmToolTip = ( NMMODULE_TEXT ) dlsym ( handle, "getModuleToolTip" );

	if ( nmmnameGUI ) mod->setPluginNameGUI ( nmmnameGUI() );
	else mod->setPluginNameGUI ( m->getPluginName() );
	if ( nmmnameCore ) mod->setPluginNameCore ( nmmnameCore() );
	else mod->setPluginNameCore ( m->getPluginName() );
	if ( nmmIcon ) mod->setModuleIcon ( nmmIcon() );
	else mod->setModuleIcon ( "" );
	if ( nmmToolTip ) mod->setModuleToolTip ( nmmToolTip() );
	else mod->setModuleToolTip ( "" );
}

#include "nmgmodulefactory.moc"
