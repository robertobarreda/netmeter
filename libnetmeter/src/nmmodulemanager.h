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
#ifndef NMPARENTNMODULEMANAGER_H
#define NMPARENTNMODULEMANAGER_H

#include "nmparent.h"
#include "nmfile.h"
#include "nmmodulefactory.h"
#include "nmmodule.h"
#include <list>

/**
*@short Permit handle shared libraries.
*@author Pau Capella
*/


struct NMModuleInfo {
    	NMString modulname;
    	void *param;	
};

class NMInterpret;

class NMModuleManager: public NMParent 
{
protected:
	/*LLista on guardem tots els plugins carregats*/
	list<NMModule *> modulelist;
	/*Instànacia única de moduleManager*/
	static NMModuleManager *nmmodulemanager;
	
	
	friend class NMInterpret;	
	
protected:
	NMModuleManager(NMParent *mm_parent);

	virtual NMModule * makeModule(const NMString& path);

public:
	~NMModuleManager();
	static NMModuleManager *self();
	void loadAllModules(const NMString& path);
	void unloadAllModules();
	int loadModule(const NMString& path);
	NMModule *getModule(const NMString& classname);
	NMModule *getModule( int id);
	bool exsistModule( const NMString& classname);
	list<NMString> * getModuleList();
	

};


#endif
