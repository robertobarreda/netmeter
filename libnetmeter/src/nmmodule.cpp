/***************************************************************************
                          nmmodule.cpp  -  description
                             -------------------
    begin                : ds feb 12 2005
    copyright            : (C) 2005 by Pau Capella
    email                : pcapella@ac.upc.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

 
#include "nmmodule.h"
#include "nmmodulefactory.h"
#include <dlfcn.h>

/**
  \fn NMModule(NMString nms_name, NMParent *nm_parent)
  \brief Construct NMModule
*/
NMModule::NMModule(NMString nms_name, NMParent *nm_parent)
:NMParent(nms_name.c_str(), nm_parent),
pluginname(nms_name), filename(), capabilities(), version()
{
	handle = 0;	
	nms_status = error;

}

NMModule::~NMModule()
{
	int rc;

	if(handle != 0) {
		if (lnmi_interfaces.size()) {
			NMString msgwar = "You have interfaces loaded in plugin " + pluginname + ". I try unload.";
			cerr << NMError(NMError::warning, msgwar);
		}
		while(lnmi_interfaces.size()) {
			NMModuleInterface *plugin = lnmi_interfaces.front();
			removePluginInterface(plugin);
		}
		rc = dlclose(handle);
		if(rc) {
			NMString msgerr = "Unload module " + pluginname + " failed: " + strerror(errno); 
			cerr << NMError(NMError::fatal,msgerr);
		}
		else {
			NMString msgdebug = pluginname + " unload correctly :)";
			cerr << NMError(NMError::debug,msgdebug);
		}
	}
}

/*! 
    \fn NMModule::getHandle()
    \brief Get plugin handle. 
    \return Handle.
*/   
void* NMModule::getHandle()
{
	return handle;
}

/*! 
    \fn NMModule::getFileName()
    \brief Get plugin filename. 
    \return Filename.
*/  
NMString NMModule::getFileName()
{
	return filename;
}

/*! 
    \fn NMModule::getPluginName()
    \brief Get plugin name.
    \return Name.
*/
NMString NMModule::getPluginName()
{
	return pluginname;
}

/*!
    \fn NMModule::getCapabilities()
    \brief Get plugin capabilities.
    \return Capabilities
 */
NMString NMModule::getCapabilities()
{
	return capabilities;
}

/*!
    \fn NMModule::getVersion()
    \brief Get plugin version.
    \return Version
 */
NMString NMModule::getVersion()
{
	return version;
}


/*!
    \fn NMModule::getSatus()
    \brief Get plugin status.
    \return Status.
 */
int NMModule::getStatus()
{
	return nms_status;
}

/*! 
    \fn NMModule::setPluginName(const NMString& modulname)
    \brief Set plugin name. 
    \param modulename Classname plugin
*/
void NMModule::setPluginName(const NMString& modulename)
{
	pluginname = modulename;
}

/*! 
    \fn NMModule::setFileName(const NMString& modulefile)
    \brief Set plugin filename. 
    \param modulefile Filename.
*/
void NMModule::setFileName(const NMString& modulefile)
{
	filename = modulefile;
}

/*! 
    \fn NMModule::setHandle()
    \brief Set plugin handle. 
    \param handle.
*/
void NMModule::setHandle(void *handle)
{
	this->handle = handle;
}


/*!
    \fn NMModule::setCapabilities(const NMString& modulecap)
    \brief Set plugin capabilities. 
    \param modulecap Capabilities.
*/
void NMModule::setCapabilities(const NMString& modulecap)
{
    capabilities = modulecap;
}

/*!
    \fn NMModule::setVersion(const NMString& moduleversion)
    \brief Set plugin version.
    \param moduleversion Version.
*/
void NMModule::setVersion(const NMString& moduleversion)
{
	version = moduleversion;
}

/*!
    \fn NMModule::setStatus(const NMModuleStates& new_status)
    \brief Set plugin status.
    \param new_status Status
*/
void NMModule::setStatus (const NMModuleStates& new_status)
{
    nms_status = new_status;
}


/*!
    \fn findInterface( const unsigned int &id)
    \brief Returns the NMModuleInterface with identifier id, or 0 if there is no NMModuleInterface with this identifier.
*/
NMModuleInterface *NMModule::findPluginInterface(const unsigned int &id)
{
	list<NMModuleInterface *>::iterator iter;

	for ( iter = lnmi_interfaces.begin(); iter != lnmi_interfaces.end(); iter++ ) {
		if((*iter)->testId() == id) return (*iter);
	}	
	
	return 0;
}

/*!
    \fn NMModule::newInterface( const unsigned int &id)
    \brief This function return a new instance of NMModuleInterface with identifier id. If an instance exists with idetifier id, return this.
*/
NMModuleInterface *NMModule::newPluginInterface (const unsigned int &id)
{
	NMModuleInterface *nmi_plugin;

	if ((nmi_plugin = findPluginInterface(id)) != 0) return nmi_plugin;

	NMModuleFactory nmf_factory(this);

	nmi_plugin = nmf_factory.makePluginInterface (handle);

	//Agegim el modul com a pare
	nmi_plugin->setParent( this );
	//Li fiquem un nom
	NMString name = pluginname + "-" + NMString::number(id);
	nmi_plugin->setName(name.c_str());
	//Numero de test
	nmi_plugin->setTestId(id);
	//excutem initPlugin
	nmi_plugin->initModule();
	//L'Afegim a la llista
	lnmi_interfaces.push_back(nmi_plugin);

	#ifdef DEBUG
	cout << "DEBUG: Module inteface " << name << " is loaded." << endl;
	#endif

	return nmi_plugin;
}


/*!
    \fn NMModule::removePluginInterface (NMModuleinterface *pnmi_plugin);
    \brief Remove the NMModuleInterface plugin of module.
*/
void NMModule::removePluginInterface (NMModuleInterface *plugin)
{
	if (!findPluginInterface(plugin->testId())) return;
	#ifdef DEBUG
	cout << "DEBUG: Module Inteface " << plugin->name() << " unload." << endl;
	#endif
	lnmi_interfaces.remove(plugin);
	plugin->removeModule();
	delete plugin;
}







