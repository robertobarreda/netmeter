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
#include "nmmodulemanager.h"
#include <sys/types.h>
#include <dirent.h>

NMModuleManager *NMModuleManager::nmmodulemanager=0;

/*!
    \fn NMModuleManager::getModuleManager()
    \brief Return the instance of ModuleManager.
    \return NMModuleManager
 */
NMModuleManager * NMModuleManager::self()
{
	if(nmmodulemanager)
		return nmmodulemanager;
	return nmmodulemanager = new NMModuleManager(0); 
}


NMModuleManager::NMModuleManager(NMParent *nm_parent):NMParent("modulemanager", nm_parent)
{
}


NMModuleManager::~NMModuleManager()
{
	//unloadAllModules();
}



/*!
    \fn NMModuleManager::loadAllModules(const NMString& dir)
    \brief Load all plugins in a path.
    \param dir Directory that contains plugins.
 */
void NMModuleManager::loadAllModules(const NMString& dir)
{
    	DIR *dpath;
	struct dirent *dfile;
	int rc;
	
	//Open the plugins directory
	dpath = opendir(dir.c_str());
	if(dpath == NULL) {
		NMString msg = "NMModuleManager loadAllModules failed: ";
		msg += strerror(errno);
		cerr << NMError(NMError::fatal,msg);
                return;
	}
	
	while((dfile = readdir(dpath))) {

		
		if(!strcmp(dfile->d_name,".") || !strcmp(dfile->d_name,".."))
			continue;

		NMString nms_file = dfile->d_name;

		if(nms_file.search(".so") < 0 )  continue;		

		NMString pluginpath = dir + "/" + nms_file;
		
		rc = loadModule(pluginpath);
		if(rc < 0) {
			NMString msg = "NMModuleManager can not load ";
			msg += dfile->d_name;
			cerr << NMError(NMError::user,msg);
			continue;
		}
	}	
	closedir(dpath);
}

/*!
    \fn NMModuleManager::makeModule(const NMString& path)
    \brief Load the plugin in the indicated path
    \param path Directory of the plugin
    \return The plugin it loaded correctly, otherwise return 0
 */
NMModule * NMModuleManager::makeModule(const NMString& path)
{
	NMModuleFactory factory(this);
	return factory.makeModule(path);
}

/*!
    \fn NMModuleManager::loadModule(const NMString& path)
    \brief Load all plugins in a path.
    \param path Directory of the plugins.
    \return 0 if plugin is load correctly, otherwise return -1
 */
int NMModuleManager::loadModule(const NMString& path)
{
		
	NMModule *modul;
	
	modul = makeModule(path);
	if(modul == NULL) {
		cerr << "loadPlugin failed: " << path.c_str() << " isn't a valid plugin" << '\n';
		return -1;
	}	
	
// 	#ifdef DEBUG
// 	cout << "\n*****************INFO NMMODULEMANAGER *********************" << '\n';
// 	cout << "Load: " << modul->getPluginName() << '\n';
// 	cout << "Fitxer: " <<   modul->getFileName() << "\n";
// 	cout << "Capabilities: " << modul->getCapabilities() << '\n';
// 	cout << "***********************************************************" << '\n';
// 	#endif
	
	if(exsistModule(modul->name())) {
		cerr << "NMModuleManger loadModules failed: This module " << modul->name() << " is loaded! " << '\n';
		delete modul;
		return -1;	
	}
	
	modulelist.push_back(modul);
	return 0;	
}


/*!
    \fn NMModuleManager::unloadAllModules()
    \brief Unload all the plugins.
 */
void NMModuleManager::unloadAllModules()
{
	while(modulelist.size()) {
		NMModule *modul;
		modul = modulelist.front();
		modulelist.pop_front();
		delete modul;
	}
}


/*!
    \fn NMModuleManager::getModule(const NMString& classname)
    \brief Get the plugin "classname".
    \param classname Classname of the plugin.
    \return An instance of module or NULL if error.
 */
NMModule *NMModuleManager::getModule( const NMString& classname)
{
	list<NMModule *>::iterator iter;
	 for ( iter = modulelist.begin(); iter != modulelist.end(); ++iter ) {
	 	if(!strcmp((*iter)->name(), classname.c_str()))
			return *iter; 
	 }
	 return NULL;
}


/*!
    \fn NMModuleManager::getModule(int id)
    \brief Get the plugin identify for id.
    \param id
    \return An instance of module or NULL if error.
 */
NMModule *NMModuleManager::getModule( int id)
{
	list<NMModule *>::iterator iter;
	
	iter = modulelist.begin();
	for ( int index=0; index < id; index++) {
		iter++;
		if(iter == modulelist.end()) return NULL;
		
	 }
	 return *iter;
}




/*!
\fn NMModuleManager::exsistModule(const NMString& classname)
    \brief Exsist a module whit name classname
    \param classname Classname of the module.
    \return true if module exsist or false if not.
*/

bool NMModuleManager::exsistModule( const NMString& classname)
{
	list<NMModule *>::iterator iter;
	 for ( iter = modulelist.begin(); iter != modulelist.end(); ++iter ) {
	 	if(!strcmp((*iter)->name(), classname.c_str()))
			return true; 
	 }
	 return false;
}


/*!
\fn NMModuleManager::getModuleList() 
\brief Returns a list with the all the module names loaded in the module manager
 
*/
list<NMString> * NMModuleManager::getModuleList()
{
	list<NMString> * resultList = new list<NMString>();
	list<NMModule *>::iterator iter;

	for ( iter = modulelist.begin(); iter != modulelist.end(); ++iter )
		resultList->push_back((*iter)->name());
	return resultList;
}

