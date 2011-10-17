/***************************************************************************
 *   Copyright (C) 2005 by Pau Capella   *
 *   pcapella@ac.upc.edu   *
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

#include <iostream>
#include <cstdlib>

#include <nmparent.h>
#include <nmmodulemanager.h>
#include <nmtaskmanager.h>
#include <nmexternallocal.h>
#include <nmexternalremote.h>
#include <nmhost.h>
#include <nmdns.h>
#include <nmeventmanager.h>
#include <nmsocketremote.h>
#include <nmthread.h>
#include <nminterface.h>
#include <nmxml.h>
#include <nmsettings.h>

using namespace std;

static const char *_version = "0.1 Last buid:30/03/2006";

static void usage()
{
	cout << "Version " << _version << '\n';
	cout << "Usage NetMeter options" << '\n';
	cout << "\t-p   port  listen port for NetMeter; default = 6621" << '\n';
// 	cout << "\t-l   file  log file" << '\n';
	cout << "\t-h   print this message and exit" << '\n';
}


int main ( int argc, char *argv[] )
{

#if 1
//Pass d'arguments....
	for ( int i=1; i<argc; ++i )
	{
		if ( strcmp ( argv[i],"-p" ) ==0 )
		{
			NMSettings::self()->setNMPort ( NMString ( argv[++i] ).toInt() );
		}
// 		else if (strcmp(argv[i],"-l")==0) {
// 			cout << "log file: " << argv[++i] << endl;
// 		}
		else
		{
			usage();
			return 1;
		}
	}


//Engegem la interface que escolta els plugins
	NMInterface *interface = NMInterface::self();
	if ( !interface->ok() )
	{
		cerr << NMError ( NMError::fatal, "Interface is not up" );
		usleep ( 500 );
		delete interface;
		return EXIT_SUCCESS;
	}

//Engegem el gestor de moduls amb els moduls
	NMModuleManager *modulemanager = NMModuleManager::self();

	if ( PLUGIN_DIR != NULL )
		modulemanager->loadAllModules ( PLUGIN_DIR );
	else
		cerr << NMError ( NMError::fatal, "No plugins are installed, continuing anyway" );

	/*modulemanager->loadModule("plugins/libnmping/.libs/libnmping.so.0.0.0");
	modulemanager->loadModule("plugins/libnmtrace/.libs/libnmtrace.so.0.0.0");
	modulemanager->loadModule("plugins/libnmexemple/.libs/libnmexemple.so");
	//   modulemanager->loadModule("plugins/liboreneta/.libs/liboreneta.so.0.0.0");
	modulemanager->loadModule("plugins/libnmmgen/.libs/libnmmgen.so.0.0.0");
	modulemanager->loadModule("plugins/libnmexemple/.libs/libnmexempleadder.so");
	modulemanager->loadModule("plugins/libnmsnooper/.libs/libnmsnooper.so");
	//	modulemanager->loadModule("plugins/libnmexemple/.libs/libnmsendfilesecure.so");
	*/
//Engegem el gestor de tasques i es quedem a l'espera de rebre tasques...
	NMTaskManager *taskmanager = NMTaskManager::self();
	taskmanager->runLoop();

	modulemanager->unloadAllModules();


	delete interface;
	delete taskmanager;
	delete modulemanager;
	delete NMSettings::self();

#endif

	return EXIT_SUCCESS;

}
