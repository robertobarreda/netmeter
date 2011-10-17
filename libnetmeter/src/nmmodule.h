/***************************************************************************
                          nmmodule.h  -  description
                             -------------------
    begin                : ds feb 12 2005
    copyright  const unsigned int &id          : (C) 2005 by Pau Capella
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

#ifndef NMMODULE_H
#define NMMODULE_H

#include <list>
#include "nmparent.h"
#include "nmmoduleinterface.h"

/**
  *@author Pau Capella
  *@short Permit storage all the information related with a shared library.
  */


class NMModule : public NMParent  {

public:

	/**  \enum NMModuleStates
	     \param loaded Plugin is load and initialized.
	     \param error Plugin contains error.
	     \param dirty Plugin has loaded interface objects.
	*/
	enum NMModuleStates {loaded, error, dirty }; 


	NMModule(NMString nms_name=0, NMParent *nm_parent=0);
	~NMModule();
	
	NMString getPluginName();
	NMString getFileName();
	NMString getCapabilities();
	NMString getVersion();
	void *getHandle();
	int getStatus();


	void setPluginName (const NMString& modulename);
	void setFileName (const NMString& modulefile);
	void setCapabilities (const NMString& modulecap);
	void setVersion (const NMString& moduleversion);
	void setHandle (void *modulehandle);
	void setStatus (const NMModuleStates& status);

	//Metodes
	NMModuleInterface *newPluginInterface (const unsigned int &id);
	void removePluginInterface (NMModuleInterface *plugin);
	NMModuleInterface *findPluginInterface (const unsigned int &id);

	
	NMString pluginname; 
	NMString filename;
	NMString capabilities;
	NMString version;
	void *handle;
	NMModuleStates nms_status;
	
	//Aquesta llista conte els objectes que es van creant
	list<NMModuleInterface *> lnmi_interfaces;

};
#endif
