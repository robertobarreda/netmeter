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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "nmgmain.h"
#include <nmgmodulemanager.h>

#include <iostream>
using namespace std;


/**
 * Loads the modules
 */
void NMGMain::loadModules()
{
	moduleManager = NMGModuleManager::self();
	moduleManager->setLoadingModulesStartup ( true );

	/* Quan estiguin tots a un directori es podrá fer directament
		moduleManager->loadAllModules("/modules"); /o el path que sigui
	*/

// Just for develop purpose
/*#ifdef DEVEL
	if ( moduleManager->loadModule ( "plugin/libnmgtrace/.libs/libnmgtrace.so" ) == 0 )
		cout << "Module NMTrace loaded" << endl;
	else cerr << "Error while loading NMTrace module" << endl;
#endif*/
	moduleManager->loadAllModules ( PLUGIN_DIR );
	cout << "*** Plugins Loaded Automatically. Starting the application ***" << endl;
	moduleManager->setLoadingModulesStartup ( false );
}

/**
 * Contructor of the class
 * @param a
 * @return
 */
NMGMain::NMGMain ( QApplication &a )
{
	NMGApplicationInformation::createConfigurationDirectory();
	fileManager = NMGConfigurationFileManager::self();
	loadModules();
	nmConnection = NMGConnection::getConnection();
	nmConnection->start();
	a.connect ( &a, SIGNAL ( lastWindowClosed() ), nmConnection, SLOT ( end() ) );
	hostManager = NMGHostInformationManager::self();
	preferencesManager = NMGPreferencesManager::self();
}

/**
 * Destroyer of the class
 * @return
 */
NMGMain::~NMGMain()
{
//	delete moduleManager;
	NMGApplicationInformation * ai = NMGApplicationInformation::getApplicationInformation();
	delete ai;
	delete hostManager;
	delete preferencesManager;
	delete nmConnection;
	delete fileManager;
}


