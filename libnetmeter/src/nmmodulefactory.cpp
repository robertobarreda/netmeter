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
#include "nmmodulefactory.h"
#include <dlfcn.h>

NMModuleFactory::NMModuleFactory(NMParent *nm_parent):NMParent("modulefactory", nm_parent)
{

}


NMModuleFactory::~NMModuleFactory()
{
		
}
/**
  \fn NMModuleFactory::makeModule(const NMString *nms_path)
  \brief Create a module.
  \param nms_path Path to shared library 
  \return A new NMModule with all load.
*/
NMModule *NMModuleFactory::makeModule(const NMString nms_path)
{
	//Tipos de funcions definides a NMModuleInterface.h
	NMMODULE_TEXT nmmname;
	NMMODULE_TEXT nmmcapability;
	NMMODULE_TEXT nmmversion;
	
	void *handle;
	
	#ifdef DEBUG
	cout << "INFO: Load plugin... " << nms_path.c_str() << '\n';
	#endif
	handle = NULL;
	handle = dlopen(nms_path.c_str(), RTLD_LAZY);
	if(!handle) {
                cerr << "NMModuleFactory Error: dlopen " << dlerror() << '\n';
		return NULL;
	}
	else {
		nmmname = (NMMODULE_TEXT) dlsym(handle, "getModuleName"); 	
        	if(!nmmname) {
                	cerr << "NMModuleFactory Error: " << dlerror() << '\n';
                	dlclose(handle);
			return NULL;
        	}
		nmmcapability = (NMMODULE_TEXT) dlsym(handle, "getModuleCapabilities");
		if(!nmmcapability) {
                	cerr << "NMModuleFactory Error: " << dlerror() << '\n';
                	dlclose(handle);
			return NULL;
        	}
		nmmversion = (NMMODULE_TEXT) dlsym(handle, "getModuleVersion");
		if(!nmmversion) {
                	cerr << "NMModuleFactory Error: " << dlerror() << '\n';
                	dlclose(handle);
			return NULL;
        	}				
			
	}
	
	//Creem una inst�cia de m�ul que sera la qui passem a la classe NMModuleManager
	//NMModule *nmmodule=new NMModule(nmmname(), this->parent());
	NMModule *nmmodule=createNewModule(nmmname(), this->parent());
	nmmodule->setCapabilities(nmmcapability());
	nmmodule->setVersion(nmmversion());
	nmmodule->setFileName(nms_path);
	nmmodule->setHandle(handle);
	nmmodule->setStatus(NMModule::loaded);
	
	makeModulePlantilla(handle,nmmodule);
	
	return nmmodule;
	
}

/**
  \fn NMModuleFactory::createNewModule(NMString name, NMParent * parent)
  \brief Creates and empty module
  \param name module name
  \param parent Parent of the new module
  \return A new instance of NMModule
*/
NMModule* NMModuleFactory::createNewModule(NMString name, NMParent * parent)
{
	return new NMModule(name, parent);
}

/**
  \fn NMModuleFactory::makePluginInterface(void *handle)
  \brief Make plugin interface
  \param handle Plugin handle. 
  \return A new instance of NMModuleInterface for this handle.
*/
NMModuleInterface * NMModuleFactory::makePluginInterface(void *handle)
{
	NMMODULE_INTERFACE nmi_interface;
	//NMModuleInterface *nmi;
	
	if(!handle) {
		cerr << "NMModuleFactory Error: No handle find!. Is module load?" << '\n';
		return 0;
	} 
	nmi_interface = (NMMODULE_INTERFACE) dlsym(handle, "getInterface");
	if(!nmi_interface) {
                cerr << "NMModuleFactory Error: dlsym" << dlerror() << '\n';
                dlclose(handle);
		return NULL;
        }
	return nmi_interface();	
}

